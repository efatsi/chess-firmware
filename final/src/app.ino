SYSTEM_MODE(SEMI_AUTOMATIC);

#include "lib/GameState.h"
#include "lib/Screen.h"
#include "lib/Board.h"
#include "lib/Constants.h"
#include "lib/Api.h"

GameState gameState;
Screen screen;
Board board;
Api api;

int ledPin    = D7;
int playerPin = D6;

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

  screen.introSequence(homePlayer == WHITE ? "WHITE" : "BLACK");

  api.init(homePlayer, &screen, &board, &gameState);
  api.connectToTheInternets();
}

void loop() {
  board.determineState(gameState.currentPlayer);

  if (board.unstable()) {
    screen.rawPrint("Fix positions:", board.requiredFixes);
  } else if (board.moveDetected()) {
    confirmChanges(board.moveString);
  }

  // board.printBinary();
  // board.printReadings();
  board.printFullStatus();

  delay(50);
}

void confirmChanges(String move) {
  if (homePlayer == gameState.currentPlayer) {
    screen.printMove(gameState.currentPlayer, move);
    api.postMove(move);
  } else {
    // TODO: check if move matches instruction
    screen.printMove(gameState.currentPlayer, "satisfied");
  }

  board.confirmChanges(gameState.currentPlayer);

  gameState.nextPlayer();
}

int handleMove(String instruction) {
  screen.printMove(gameState.currentPlayer, instruction + " ...");
  digitalWrite(ledPin, HIGH);

  return 1;
}
