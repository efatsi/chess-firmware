#include "lib/Constants.h"

#include <LiquidCrystal.h>

class Screen {
public:
    LiquidCrystal *lcd;

    Screen() {
      lcd = new LiquidCrystal(A0, A1, A2, A3, A4, A5);
    }

    void init(String player) {
      lcd->begin(16, 2);
      _introSequence(player);
    }

    void printMove(int player, String move) {
      String playerString = player == WHITE ? "White" : "Black";
      String line_1 = " " + playerString + "'s Move:";
      String line_2 = "   " + move;

      _rawPrint(line_1, line_2);
    }

  private:

    void _rawPrint(String line_1, String line_2) {
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print(line_1);
      lcd->setCursor(0, 1);
      lcd->print(line_2);
    }

    void _introSequence(String player) {
      _rawPrint("   Yay Chess!", "       - fat$$");

      delay(1000);
      _wipeClean();

      _rawPrint(" Playing As:", "      " + player);

      delay(1500);
      _wipeClean();
    }

    void _wipeClean() {
      lcd->setCursor(0, 0);
      for (int i = 0; i < 32; i++) {
        if (i == 16) lcd->setCursor(0, 1);

        lcd->print(" ");
        if (!(i >= 16 && i < 23)) delay(50);
      }
    }
};
