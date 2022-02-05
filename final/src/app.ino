#include "lib/Screen.h"
#include "lib/Board.h"
#include "lib/Constants.h"

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

    Serial.println("Posting: /games/4/move?move=" + move);

    String response;
    int statusCode = client.post("/games/4/move?move=" + move, &response);

    Serial.println("Response got");
    Serial.println(statusCode);
    Serial.println(response);
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

// Particle Functions
// int updateLight(String command) {
//   if (command == "on") {
//     digitalWrite(ledPin, HIGH);
//   } else {
//     digitalWrite(ledPin, LOW);
//   }
//
//   return 1;
// }

int handleMove(String instruction) {
  screen.printMove(currentPlayer, instruction + " ...");

  return 1;
}
