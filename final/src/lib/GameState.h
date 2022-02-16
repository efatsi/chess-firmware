#include "lib/Constants.h"

class GameState {
public:
  int currentPlayer;
  String currentMessage;

  void nextPlayer() {
    currentPlayer = WHITE + BLACK - currentPlayer;
  }
};
