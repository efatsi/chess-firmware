#include "lib/api/Request.h"

class Api {
public:
  Screen* screen;
  Board* board;
  GameState* gameState;
  int homePlayer;

  String gameId;
  Request request;

  int ledPin = D7;

  void init(int h, Screen* s, Board* b, GameState* gs) {
    homePlayer = h;
    screen = s;
    board = b;
    gameState = gs;
  }

  void connectToTheInternets() {
    screen->rawPrint(" Connecting...");

    Particle.connect();

    _waitForConnection();
    _connectToBoard();
  }

  void postMove(String move) {
    String url = "/games/" + gameId + "/move?move=" + move;
    Response response = request.post(url);

    if (response.success()) {
      Serial.println("Successful move!");
      digitalWrite(ledPin, LOW);
    } else {
      Serial.println("Failed response: " + response.error());
      screen->rawPrint("Error: ", response.error());
    }
  }

private:
  void _waitForConnection() {
    int dotCount = 3;
    while (!Particle.connected()) {
      delay(100);

      String toPrint = " Connecting";
      dotCount = (dotCount % 3) + 1;
      for (int i = 0; i < dotCount; i++) {
        toPrint += ".";
      }
      screen->rawPrint(toPrint);
    }
  }

  void _connectToBoard() {
    String color = homePlayer == WHITE ? "white" : "black";
    String params = "?device_id=" + System.deviceID() + "&color=" + color;
    String url = "/games/connect" + params;
    Response response = request.post(url);

    if (response.success()) {
      // Store game_id for move posting
      gameId = response.dig("game_id");
      Serial.println("Got a game_id: " + gameId);

      // Set current player
      String player = response.dig("player");
      gameState->currentPlayer = (player == "white" ? WHITE : BLACK);

      // Set board fen state
      String fen = response.dig("fen");
      board->resetState(fen);

      // Save and print current game message
      gameState->currentMessage = response.message();
      screen->rawPrint("   Connected!", response.message());
    } else {
      Serial.println("Failed response: " + response.error());
      screen->rawPrint("Error: ", response.error());
    }
  }
};
