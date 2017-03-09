int dataPin   = D6;
int clockPin  = D4;
int latchPin  = D5;
int clearPin  = D3;

int sensorPin = A5;

class Position {
  #define UNSTABLE_OFF    0
  #define UNSTABLE_ON     1
  #define UNSTABLE_OFF_ON 2
  #define STABLE_OFF      3
  #define STABLE_ON       4
  #define STABLE_OFF_ON   5
  #define CONFIRMED       6

  #define EMPTY 0
  #define WHITE 1
  #define BLACK 2

  public:
    String position;
    int    occupied;
    int    state;
    int    index;
    int    value;
    long   lastChange;
    Position(String newPosition, int newIndex, int newOccupied) {
      position   = newPosition;
      state      = CONFIRMED;
      index      = newIndex;
      occupied   = newOccupied;
      value      = occupied ? 1 : 0;
      lastChange = 0;
    }

    void setValue(int newValue, int player, long currentTime) {
      value      = newValue;
      lastChange = currentTime;

      if (newValue) {
        if (state == UNSTABLE_OFF || state == STABLE_OFF) {
          if (occupied == player) {
            // changed their mind
            state = CONFIRMED;
          } else {
            // player's piece of taking other's
            state = UNSTABLE_OFF_ON;
          }
        } else {
          state = UNSTABLE_ON;
        }
      } else {
        if (state == UNSTABLE_ON) {
          // on blip - likely just a passing piece;
          state = CONFIRMED;
        } else {
          state = UNSTABLE_OFF;
        }
      }
    }

    void checkStability() {
      if (_unstable() && _changeIsOld()) {
        switch (state) {
          case UNSTABLE_OFF:
            state = STABLE_OFF;
            break;
          case UNSTABLE_ON:
            state = STABLE_ON;
            break;
          case UNSTABLE_OFF_ON:
            state = STABLE_OFF_ON;
            break;
        }
      }
    }

    void confirmState(int player) {
      switch (state) {
        case STABLE_OFF:
          occupied = EMPTY;
          break;
        case STABLE_ON:
          occupied = player;
          break;
        case STABLE_OFF_ON:
          occupied = player;
          break;
      }

      state = CONFIRMED;
    }

  private:
    bool _unstable() {
      return state == UNSTABLE_ON || state == UNSTABLE_OFF || state == UNSTABLE_OFF_ON;
    }

    bool _changeIsOld() {
      return millis() - lastChange > 1000;
    }
};

const int count = 8;

int currentPlayer = WHITE;

Position positions[count] = {
  Position("c1", 0, EMPTY),
  Position("c2", 1, EMPTY),
  Position("c3", 2, EMPTY),
  Position("c4", 3, EMPTY),
  Position("c5", 4, EMPTY),
  Position("c6", 5, EMPTY),
  Position("c7", 6, WHITE),
  Position("c8", 7, BLACK)
};

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin,  OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(clearPin, OUTPUT);
  digitalWrite(clearPin, HIGH);

  pinMode(sensorPin, INPUT);
}

void loop() {
  fetchSensorData();
  checkForMove();
}

void fetchSensorData() {
  clear();

  for (int i = 0; i < count; i++) {
    // Set first memory value HIGH, then push LOWs in.
    digitalWrite(dataPin, i == 0);
    advanceClock();
    latch();

    int sensorValue = digitalRead(sensorPin);
    Position &position = positions[i];

    if (sensorValue != position.value) {
      position.setValue(sensorValue, currentPlayer, millis());
    } else {
      position.checkStability();
    }
  }
}

void checkForMove() {
  String stableOff   = "";
  String stableOn    = "";
  String stableOffOn = "";

  for (int i = 0; i < count; i++) {
    Position &position = positions[i];

    if (position.state == STABLE_OFF) {
      stableOff = position.position;
    } else if (position.state == STABLE_ON) {
      stableOn = position.position;
    } else if (position.state == STABLE_OFF_ON) {
      stableOffOn = position.position;
    }
  }

  if (stableOff != "") {
    if (stableOn != "" || stableOffOn != "") {
      Serial.print(stableOff);
      Serial.print("-");
      Serial.print(stableOn);
      Serial.print(stableOffOn);
      Serial.println("");
      confirmChanges();
    }
  }
}

void confirmChanges() {
  for (int i = 0; i < count; i++) {
    Position &position = positions[i];
    position.confirmState(currentPlayer);
  }

  currentPlayer = WHITE + BLACK - currentPlayer;
}

void clear() {
  digitalWrite(clearPin, LOW);
  digitalWrite(clearPin, HIGH);
}

void advanceClock() {
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
}

void latch() {
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}
