#include "lib/api/Request.h"

class Api {
public:
  Screen* screen;
  Board* board;
  GameState* gameState;
  int homePlayer;

  String gameId;
  Request request;

  Api(Screen* s, Board* b, GameState* gs) {
    screen = s;
    board = b;
    gameState = gs;
  }

  void init(int h) {
    homePlayer = h;
  }

  void connectToTheInternets() {
    screen->rawPrint(" Connecting...");

    Particle.connect();

    _waitForConnection();
    _connectToBoard();
  }

  bool postMove(String move) {
    String url = "/games/" + gameId + "/move?move=" + move;
    Response response = request.post(url);

    if (response.success()) {
      gameState->currentFen = response.dig("fen");
      gameState->currentMessage = response.dig("message");

      screen->temporaryRawPrint("  Success:", "   " + move);

      return true;
    } else {
      gameState->currentFen = response.dig("fen");
      screen->rawPrint(" Error:", response.error());
      delay(1000);

      return false;
    }
  }

private:
  void _waitForConnection() {
    int dotCount = 3;
    while (!Particle.connected()) {
      delay(200);

      String toPrint = " Connecting";
      dotCount = (dotCount + 1) % 4;
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
      gameState->currentFen = fen;
      board->resetState(gameState->currentFen);

      // Save and print current game message
      gameState->currentMessage = response.message();
      screen->temporaryRawPrint("Connected!", response.message());
    } else {
      Serial.println("Failed response: " + response.error());
      screen->temporaryRawPrint("Invalid Move:", response.error());
    }
  }
};
