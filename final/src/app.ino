SYSTEM_MODE(SEMI_AUTOMATIC);

#include <JsonParserGeneratorRK.h>
#include "lib/GameState.h"
#include "lib/Screen.h"
#include "lib/Board.h"
#include "lib/Printer.h"
#include "lib/Constants.h"
#include "lib/Api.h"

GameState gameState;
Screen screen(&gameState);

Board board(&screen, &gameState);
Api api(&screen, &board, &gameState);

Printer printer(&board, &gameState);

int ledPin    = D7;
int playerPin = D6;

int homePlayer;
int awayPlayer;

bool receivedMove = false;

void setup() {
  Particle.function("other-move", handleMove);

  pinMode(ledPin, OUTPUT);
  pinMode(playerPin, INPUT);

  setHomePlayer();
  screen.introSequence(homePlayer == WHITE ? "WHITE" : "BLACK");

  api.init(homePlayer);
  api.connectToTheInternets();
}

void loop() {
  board.determineState(gameState.currentPlayer);

  if (board.moveDetected(gameState.currentPlayer)) {
    confirmChanges(board.moveString);
  }

  screen.loop();
  printer.loop();

  delay(50);
}

void confirmChanges(String move) {
  board.confirmChanges(gameState.currentPlayer);

  if (homePlayer == gameState.currentPlayer) {
    screen.printMove(gameState.currentPlayer, move);

    if (api.postMove(move)) {
      gameState.nextPlayer();
    } else {
      board.resetState(gameState.currentFen);
    }
  } else {
    board.resetState(gameState.currentFen);

    if (receivedMove) {
      receivedMove = false;
      digitalWrite(ledPin, LOW);
      screen.temporaryPrintMove(gameState.currentPlayer, "satisfied");

      gameState.nextPlayer();
    }
  }
}

void setHomePlayer() {
  if (digitalRead(playerPin)) {
    homePlayer = WHITE;
    awayPlayer = BLACK;
  } else {
    homePlayer = BLACK;
    awayPlayer = WHITE;
  }
}

int handleMove(String data) {
  Serial.println("Received Move:");
  Serial.println(data);

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
    receivedMove = true;
    digitalWrite(ledPin, HIGH);

    return 1;
  } else {
    screen.temporaryRawPrint("Invalid JSON from web", data);
    return 0;
  }
}
