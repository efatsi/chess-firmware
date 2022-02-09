#include "lib/Constants.h"

#include <LiquidCrystal.h>

class Screen {
public:
    LiquidCrystal *lcd;

    Screen() {
      lcd = new LiquidCrystal(A0, A1, A2, A3, A4, A5);
      lcd->begin(16, 2);
    }

    void printMove(int player, String move) {
      String playerString = player == WHITE ? "White" : "Black";
      String line_1 = " " + playerString + "'s Move:";
      String line_2 = "   " + move;

      rawPrint(line_1, line_2);
    }

    void rawPrint(String line_1) { rawPrint(line_1, ""); }
    void rawPrint(String line_1, String line_2) {
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print(line_1);
      lcd->setCursor(0, 1);
      lcd->print(line_2);
    }

    void introSequence(String player) {
      rawPrint("   Yay Chess!", "       - fat$$");

      delay(1000);
      _wipeClean();

      rawPrint(" Playing As:", "      " + player);

      delay(1500);
      _wipeClean();
    }

  private:

    void _wipeClean() {
      lcd->setCursor(0, 0);
      for (int i = 0; i < 32; i++) {
        if (i == 16) lcd->setCursor(0, 1);

        lcd->print(" ");
        if (!(i >= 16 && i < 23)) delay(50);
      }
    }
};
