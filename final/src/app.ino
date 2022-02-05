#include "lib/Screen.h"
#include "lib/Board.h"
#include "lib/Constants.h"

#include "utils/rest_client.h"
#include <JsonParserGeneratorRK.h>

RestClient client = RestClient("6991-2601-5c2-201-7a90-1caf-5e81-96ed-61fd.ngrok.io");

Screen screen;
Board board;

// technically an int, but used as a string
String gameId;

int ledPin    = D7;
int playerPin = D6;

int currentPlayer = WHITE;
int waitingPlayer = BLACK;

int homePlayer;
int awayPlayer;

// parse max of 1024 bytes w/ 10 tokens (hopefully this holds everything)
JsonParserStatic<1024, 10> parser;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
  Particle.function("other-move", handleMove);

  pinMode(ledPin, OUTPUT);
  pinMode(playerPin, INPUT);

  if (digitalRead(playerPin)) {
    homePlayer = WHITE;
    awayPlayer = BLACK;
  } else {
    homePlayer = BLACK;
    awayPlayer = WHITE;
  }

  board.init(homePlayer, awayPlayer);
  screen.init(homePlayer == WHITE ? "WHITE" : "BLACK");

  Particle.connect();

  requestConnect();
}

void loop() {
  board.determineState(currentPlayer);

  if (board.moveDetected()) {
    confirmChanges(board.moveString);
  }

  // board.printBinary();
  // board.printReadings();
  board.printFullStatus();
  delay(50);
}

void confirmChanges(String move) {
  // Print move locally
  if (homePlayer == currentPlayer) {
    screen.printMove(currentPlayer, move);

    postMove(move);
  } else {
    // TODO: check if move matches instruction
    screen.printMove(currentPlayer, "satisfied");
  }

  // Reset board
  board.confirmChanges(currentPlayer);

  // Switch players
  waitingPlayer = currentPlayer;
  currentPlayer = WHITE + BLACK - currentPlayer;
}

int handleMove(String instruction) {
  screen.printMove(currentPlayer, instruction + " ...");
  digitalWrite(ledPin, HIGH);

  return 1;
}

void requestConnect() {
  // Make request
  String color = currentPlayer == WHITE ? "White" : "Black";
  String params = "?device_id=" + System.deviceID() + "&color=" + color;
  String url = "/games/connect" + params;
  Serial.println("Posting: " + url);

  String response;
  int statusCode = client.get(url, &response);

  Serial.println("Response got");
  Serial.println(statusCode);
  Serial.println(response);
  Serial.println();

  // Parse response
  parser.clear();
  parser.addString(response);
  if (parser.parse()) {
    bool success;
    parser.getOuterValueByKey("success", success);

    if (success) {
      // Store gameId on global variable
      parser.getOuterValueByKey("game_id", gameId);

      String message;
      parser.getOuterValueByKey("message", message);
      screen.rawPrint("   Connected!", message);
    } else {
      String message;
      parser.getOuterValueByKey("message", message);
      screen.rawPrint("Error: ", message);
    }
  } else {
    // TODO: handle failed parse
  }
}

void postMove(String move) {
  // Make request
  String url = "/games/" + gameId + "/move?move=" + move;
  Serial.println("Posting: " + url);

  String response;
  int statusCode = client.post(url, &response);

  Serial.println("Response got");
  Serial.println(statusCode);
  Serial.println(response);
  Serial.println();

  // Parse response
  parser.clear();
  parser.addString(response);
  if (parser.parse()) {
    bool success;
    parser.getOuterValueByKey("success", success);

    if (success) {
      // move succeeded, turn off player light
      digitalWrite(ledPin, LOW);
    } else {
      String message;
      parser.getOuterValueByKey("message", message);
      screen.rawPrint("Error: ", message);
    }
  } else {
    // TODO: handle failed parse
  }
}
