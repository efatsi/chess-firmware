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

  board.init(&screen, &gameState);
  api.init(homePlayer, &screen, &board, &gameState);
  api.connectToTheInternets();
}

void loop() {
  board.determineState(gameState.currentPlayer);

  if (board.moveDetected(gameState.currentPlayer)) {
    confirmChanges(board.moveString);
  }

  screen.loop();

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
    screen.temporaryPrintMove(gameState.currentPlayer, "satisfied");
  }

  gameState.nextPlayer();
}

int handleMove(String data) {
  JsonParser parser;
  parser.clear();
  parser.addString(data);

  if (parser.parse()) {
    // Store new game data
    parser.getOuterValueByKey("fen", gameState.currentFen);
    parser.getOuterValueByKey("message", gameState.currentMessage);

    // Print move from other player
    String instruction;
    parser.getOuterValueByKey("move", instruction);
    screen.printMove(gameState.currentPlayer, instruction + " ...");

    // Turn on light - indicating it's home player's turn
    digitalWrite(ledPin, HIGH);

    return 1;
  } else {
    screen.temporaryRawPrint("Invalid JSON from web", data);
    return 0;
  }
}
