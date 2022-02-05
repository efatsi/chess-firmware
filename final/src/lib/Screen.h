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

    void printPlayerMove(String player, String move) {
      lcd->clear();

      lcd->print(" " + player + "'s Move:");
      lcd->setCursor(0, 1);
      lcd->print("    " + move);
    }

    void printMove(String player, String move) {
      lcd->clear();

      lcd->print(" " + player + "'s Move:");
      lcd->setCursor(0, 1);
      lcd->print("    " + move);
    }

    void rawPrint(String output) {
      lcd->clear();
      lcd->print(output);
    }

  private:

    void _introSequence(String player) {
      lcd->print("   Yay Chess!");
      lcd->setCursor(0, 1);
      lcd->print("       - fat$$");

      delay(1000);
      _wipeClean();

      lcd->setCursor(0, 0);
      lcd->print(" Playing As:");
      lcd->setCursor(0, 1);
      lcd->print("      ");
      lcd->print(player);

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
