#include "lib/Screen.h"
#include "lib/Board.h"

#include "utils/rest_client.h"

RestClient client = RestClient("6991-2601-5c2-201-7a90-1caf-5e81-96ed-61fd.ngrok.io");

Screen screen;
Board board;

int ledPin    = D7;
int playerPin = D6;

int currentPlayer = WHITE;
int waitingPlayer = BLACK;

int homePlayer;
int awayPlayer;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
  Particle.function("screen", updateScreen);
  Particle.function("light", updateLight);

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
  String player = currentPlayer == WHITE ? "White" : "Black";
  if (homePlayer == currentPlayer) {
    screen.printMove(player, move);

    Serial.println("Posting: /games/4/move?move=" + move);

    String response;
    int statusCode = client.post("/games/4/move?move=" + move, &response);

    Serial.println("Response got");
    Serial.println(statusCode);
    Serial.println(response);
  } else {
    screen.printMove(player, move);
    // screen.rawPrint("   satisfied");
  }

  // Reset board
  board.confirmChanges(currentPlayer);

  // Switch players
  waitingPlayer = currentPlayer;
  currentPlayer = WHITE + BLACK - currentPlayer;
}

// Particle Functions
int updateLight(String command) {
  if (command == "on") {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  return 1;
}

int updateScreen(String player) {
  // TODO: split on deliminator to split player and move
  String move = "g4 - h4";
  screen.printPlayerMove(player, move);
  return 1;
}
