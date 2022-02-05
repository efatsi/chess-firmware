#include "Position.h"
#include "ChangedPosition.h"

class Board {
public:
  Position positions[64] = {
    Position("a8", BLACK),
    Position("b8", BLACK),
    Position("c8", BLACK),
    Position("d8", BLACK),
    Position("e8", BLACK),
    Position("f8", BLACK),
    Position("g8", BLACK),
    Position("h8", BLACK),

    Position("a7", BLACK),
    Position("b7", BLACK),
    Position("c7", BLACK),
    Position("d7", BLACK),
    Position("e7", BLACK),
    Position("f7", BLACK),
    Position("g7", BLACK),
    Position("h7", BLACK),

    Position("a6", EMPTY),
    Position("b6", EMPTY),
    Position("c6", EMPTY),
    Position("d6", EMPTY),
    Position("e6", EMPTY),
    Position("f6", EMPTY),
    Position("g6", EMPTY),
    Position("h6", EMPTY),

    Position("a5", EMPTY),
    Position("b5", EMPTY),
    Position("c5", EMPTY),
    Position("d5", EMPTY),
    Position("e5", EMPTY),
    Position("f5", EMPTY),
    Position("g5", EMPTY),
    Position("h5", EMPTY),

    Position("a4", EMPTY),
    Position("b4", EMPTY),
    Position("c4", EMPTY),
    Position("d4", EMPTY),
    Position("e4", EMPTY),
    Position("f4", EMPTY),
    Position("g4", EMPTY),
    Position("h4", EMPTY),

    Position("a3", EMPTY),
    Position("b3", EMPTY),
    Position("c3", EMPTY),
    Position("d3", EMPTY),
    Position("e3", EMPTY),
    Position("f3", EMPTY),
    Position("g3", EMPTY),
    Position("h3", EMPTY),

    Position("a2", WHITE),
    Position("b2", WHITE),
    Position("c2", WHITE),
    Position("d2", WHITE),
    Position("e2", WHITE),
    Position("f2", WHITE),
    Position("g2", WHITE),
    Position("h2", WHITE),

    Position("a1", WHITE),
    Position("b1", WHITE),
    Position("c1", WHITE),
    Position("d1", WHITE),
    Position("e1", WHITE),
    Position("f1", WHITE),
    Position("g1", WHITE),
    Position("h1", WHITE)
  };

  const int masterPins[3] = {D0, D1, D2};
  const int slavePins[3]  = {D3, D4, D5};
  const int sensorPin = A6;

  int currentPlayer;

  int allReadings[64];
  int upperNorm;
  const int lowPercentage = 15;

  int stableDownCount = 0;
  int stableUpCount   = 0;
  int allUpCount      = 0;
  int allDownCount    = 0;
  int unstableCount   = 0;
  ChangedPosition ups[5];
  ChangedPosition downs[5];

  String moveString;

  Board() {}

  void init(int homePlayer, int awayPlayer) {
    pinMode(D0, OUTPUT);
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);

    pinMode(sensorPin, INPUT);
  }

  void determineState(int cp) {
    currentPlayer = cp;

    _fetchSensorData();
    _calculateNorm();
    _setStatuses();

    _countUpsAndDowns();
    _verifyStatuses();
  }

  bool moveDetected() {
    if (unstableCount > 0) return false;

    int waitingPlayer = WHITE + BLACK - currentPlayer;
    if (stableUpCount == 1 && stableDownCount == 1) {
      if (ups[0].wasOccupiedBy == currentPlayer && downs[0].wasOccupiedBy == waitingPlayer) {
        // current captured waiting
        moveString = ups[0].position + "x" + downs[0].position;
        return true;
      } else if (ups[0].wasOccupiedBy == currentPlayer && downs[0].wasOccupiedBy == EMPTY) {
        // moved to empty space
        moveString = ups[0].position + "-" + downs[0].position;
        return true;
      } else {
        // TODO: en pessant / castle / orrr something's not right
      }
    }

    return false;
  }

  void confirmChanges(int currentPlayer) {
    for (int i = 0; i < 64; i++) {
      Position &position = positions[i];
      position.confirmState(currentPlayer);
    }
  }

  void printReadings() {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        int index = (i * 8) + j;
        Position &position = positions[index];

        Serial.print(position.reading);
        Serial.print("\t");
      }
      Serial.println();
    }
    Serial.println();
  }

  void printBinary() {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        int index = (i * 8) + j;
        Position &position = positions[index];

        Serial.print(position.value);
        Serial.print(" ");
      }

      Serial.println();
    }
    Serial.println();
  }

  void printFullStatus() {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        int index = (i * 8) + j;
        Position &position = positions[index];

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

private:

  void _fetchSensorData() {
    for (int i = 0; i < 8; i++) {
      _selectSlavePin(i);

      for (int j = 0; j < 8; j++) {
        _selectMasterPin(j);

        // account for flipped y-axis sensors
        int cleanI = j >= 4 ? 8 - 1 - i : i;
        int index = (cleanI * 8) + j;
        Position &position = positions[index];

        int reading = analogRead(sensorPin);

        position.reading = reading;
        allReadings[(i * 8) + j] = reading;
      }
    }
  }

  void _calculateNorm() {
    _sort(allReadings, 64);

    // take average of upper half
    int sum = 0;
    for (int i = 0; i < 32; i++) {
      sum += allReadings[64 - 1 - i];
    }
    upperNorm = sum / 32;
  }

  void _setStatuses() {
    long currentTime = millis();

    for (int i = 0; i < 64; i++) {
      Position &position = positions[i];
      bool newValue = (position.reading * 100 / upperNorm) < lowPercentage;

      if (newValue != position.startValue() || newValue != position.value) {
        position.setNewValue(newValue, currentTime);
      }

      position.checkStability(currentTime);
    }
  }

  void _countUpsAndDowns() {
    stableUpCount   = 0;
    allUpCount      = 0;
    stableDownCount = 0;
    allDownCount    = 0;
    unstableCount   = 0;

    for (int i = 0; i < 64; i++) {
      Position &position = positions[i];

      if (stableUpCount >= 5 || stableDownCount >= 5) {
        return;
      }

      if (position.status == STABLE_UP) {
        ups[stableUpCount] = ChangedPosition(position.position, position.occupiedBy);
        stableUpCount += 1;
        allUpCount += 1;
      } else if (position.status == UNSTABLE_UP) {
        unstableCount += 1;
        allUpCount += 1;
      } else if (position.status == STABLE_DOWN) {
        downs[stableDownCount] = ChangedPosition(position.position, position.occupiedBy);
        stableDownCount += 1;
        allDownCount += 1;
      } else if (position.status == UNSTABLE_DOWN) {
        unstableCount += 1;
        allDownCount += 1;
      }
    }
  }

  void _verifyStatuses() {
    for (int i = 0; i < 64; i++) {
      Position &position = positions[i];
      position.verifyStatus(allUpCount, allDownCount, currentPlayer);
    }
  }

  void _selectMasterPin(byte pin) {
    if (pin > 7) return;
    for (int i=0; i<3; i++) {
      if (pin & (1<<i))
      digitalWrite(masterPins[i], HIGH);
      else
      digitalWrite(masterPins[i], LOW);
    }
  }

  void _selectSlavePin(byte pin) {
    if (pin < 0 || pin > 7) return;
    for (int i=0; i<3; i++) {
      if (pin & (1<<i))
      digitalWrite(slavePins[i], HIGH);
      else
      digitalWrite(slavePins[i], LOW);
    }
  }

  void _sort(int a[], int size) {
    for(int i=0; i<(size-1); i++) {
      for(int j=0; j<(size-(i+1)); j++) {
        if(a[j] > a[j+1]) {
          int t = a[j];
          a[j] = a[j+1];
          a[j+1] = t;
        }
      }
    }
  }
};
