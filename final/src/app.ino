SYSTEM_MODE(SEMI_AUTOMATIC);

#include "lib/Screen.h"
#include "lib/Board.h"
#include "lib/Constants.h"
#include "lib/api/Request.h"

Screen screen;
Board board;

Request request;
String gameId;

int ledPin    = D7;
int playerPin = D6;

int currentPlayer = WHITE;
int waitingPlayer = BLACK;

int homePlayer;
int awayPlayer;


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

  delay(1000); // Give Particle.connect() time to block or something
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
  if (homePlayer == currentPlayer) {
    screen.printMove(currentPlayer, move);
    postMove(move);
  } else {
    // TODO: check if move matches instruction
    screen.printMove(currentPlayer, "satisfied");
  }

  board.confirmChanges(currentPlayer);

  waitingPlayer = currentPlayer;
  currentPlayer = WHITE + BLACK - currentPlayer;
}

int handleMove(String instruction) {
  screen.printMove(currentPlayer, instruction + " ...");
  digitalWrite(ledPin, HIGH);

  return 1;
}

void requestConnect() {
  String color = currentPlayer == WHITE ? "white" : "black";
  String params = "?device_id=" + System.deviceID() + "&color=" + color;
  String url = "/games/connect" + params;
  Response response = request.post(url);

  if (response.success) {
    gameId = response.dig("game_id");
    Serial.println("Got a game_id: " + gameId);
    screen.rawPrint("   Connected!", response.message);
  } else {
    screen.rawPrint("Error: ", response.message);
  }
}

void postMove(String move) {
  String url = "/games/" + gameId + "/move?move=" + move;
  Response response = request.post(url);

  if (response.success) {
    digitalWrite(ledPin, LOW);
  } else {
    screen.rawPrint("Error: ", response.message);
  }
}
