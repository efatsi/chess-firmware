// C++ parser hurts on this file without something (whitespace) before the class

class Printer {
public:
  Board* board;
  GameState* gameState;

  bool printing = false;
  int printMode = 1;

  Printer(Board* b, GameState* gs) {
    board = b;
    gameState = gs;
  }

  void loop() {
    _checkInput();

    if (printing) {
      switch (printMode) {
        case 1:
          _printFullStatus();
          break;
        case 2:
          _printBinary();
          break;
        case 3:
          _printReadings();
          break;
      }
    }
  }

private:

  void _checkInput() {
    if (Serial.available() > 0) {
      int input = Serial.read();

      // say what you got:
      Serial.print("Received input: ");
      Serial.println(input, DEC);

      switch (input) {
        case 's':
          printing = !printing;
          break;
        case '1':
          printMode = 1;
          Serial.println("Setting print mode: FullStatus");
          break;
        case '2':
          printMode = 2;
          Serial.println("Setting print mode: Binary");
          break;
        case '3':
          printMode = 3;
          Serial.println("Setting print mode: Readings");
          break;
      }
    }
  }

  void _printReadings() {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        int index = (i * 8) + j;
        Position &position = board->positions[index];

        Serial.print(position.reading);
        Serial.print("\t");
      }
      Serial.println();
    }
    Serial.println();
  }

  void _printBinary() {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        int index = (i * 8) + j;
        Position &position = board->positions[index];

        Serial.print(position.value);
        Serial.print("\t");
      }

      Serial.println();
    }
    Serial.println();
  }

  void _printFullStatus() {
    Serial.println();
    Serial.print("Up count:   ");
    Serial.println(board->allUpCount);
    Serial.print("Down count: ");
    Serial.println(board->allDownCount);

    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        int index = (i * 8) + j;
        Position &position = board->positions[index];

        switch (position.status) {
          case CONFIRMED:
            switch (position.occupiedBy) {
              case WHITE:
                Serial.print(" W  ");
                break;
              case BLACK:
                Serial.print(" B  ");
                break;
              case EMPTY:
                Serial.print(" .  ");
                break;
            }
            break;
          case UNSTABLE_UP:
            switch (position.occupiedBy) {
              case WHITE:
                Serial.print("‾W  ");
                break;
              case BLACK:
                Serial.print("‾B  ");
                break;
              case EMPTY:
                Serial.print("‾.  ");
                break;
            }
            break;
          case STABLE_UP:
            switch (position.occupiedBy) {
              case WHITE:
                Serial.print("‾W‾ ");
                break;
              case BLACK:
                Serial.print("‾B‾ ");
                break;
              case EMPTY:
                Serial.print("‾.‾ ");
                break;
            }
            break;
          case UNSTABLE_DOWN:
            switch (position.occupiedBy) {
              case WHITE:
                Serial.print("_W  ");
                break;
              case BLACK:
                Serial.print("_B  ");
                break;
              case EMPTY:
                Serial.print("_.  ");
                break;
            }
            break;
          case STABLE_DOWN:
            switch (position.occupiedBy) {
              case WHITE:
                Serial.print("_W_ ");
                break;
              case BLACK:
                Serial.print("_B_ ");
                break;
              case EMPTY:
                Serial.print("_._ ");
                break;
            }
            break;
        }
      }
      Serial.println();
    }

    Serial.println();
    Serial.println();
  }
};
