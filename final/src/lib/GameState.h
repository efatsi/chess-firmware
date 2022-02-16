#include "lib/Constants.h"

class GameState {
public:
  int currentPlayer;
  String currentMessage;
  String currentFen;

  void nextPlayer() {
    currentPlayer = WHITE + BLACK - currentPlayer;
  }
};
