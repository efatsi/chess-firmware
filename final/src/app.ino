SYSTEM_MODE(SEMI_AUTOMATIC);

#include <JsonParserGeneratorRK.h>
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

  // Check you're starting a move at the current FED state
  if (board.unstable()) {
    board.wasUnstable = true;
    screen.rawPrint("Fix positions:", board.requiredFixes);
  } else if (board.wasUnstable) {
    board.wasUnstable = false;
    screen.rawPrint(gameState.currentMessage);
  } else if (board.moveDetected(gameState.currentPlayer)) {
    // Check for a confirmed move
    confirmChanges(board.moveString);
  }

  // board.printBinary();
  // board.printReadings();
  board.printFullStatus();

  delay(50);
}

void confirmChanges(String move) {
  board.confirmChanges(gameState.currentPlayer);

  if (homePlayer == gameState.currentPlayer) {
    screen.printMove(gameState.currentPlayer, move);

    bool success = api.postMove(move);
    if (!success) {
      board.resetState(gameState.currentFen);
      return;
    }
  } else {
    board.resetState(gameState.currentFen);

    if (board.stable) {
      screen.printMove(gameState.currentPlayer, "satisfied");
    }
  }

  gameState.nextPlayer();
}

int handleMove(String data) {
  JsonParser parser;
  parser.clear();
  parser.addString(data);

  if (parser.parse()) {
    // Store new fen string
    parser.getOuterValueByKey("fen", gameState.currentFen);

    // Print move from other player
    String instruction;
    parser.getOuterValueByKey("move", instruction);
    screen.printMove(gameState.currentPlayer, instruction + " ...");

    // Turn on light - indicating it's home player's turn
    digitalWrite(ledPin, HIGH);

    return 1;
  } else {
    screen.rawPrint("Invalid JSON from web", data);
    return 0;
  }
}
