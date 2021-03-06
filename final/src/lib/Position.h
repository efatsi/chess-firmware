class Position {
  // status
  #define CONFIRMED     0
  #define UNSTABLE_UP   1
  #define STABLE_UP     2
  #define UNSTABLE_DOWN 3
  #define STABLE_DOWN   4

  #define CONFIRM_TIME 1000

public:
  String position;
  int    reading;
  bool   value;

  int    status;
  int    occupiedBy;

  bool   changed;
  long   lastChange;

  Position(String newPosition, int newOccupiedBy) {
    position = newPosition;
    resetState(newOccupiedBy);
  }

  void resetState(int newOccupiedBy) {
    status     = CONFIRMED;
    occupiedBy = newOccupiedBy;
    value      = occupiedBy ? true : false;
    changed    = false;
    lastChange = 0;
  }

  void setNewValue(int newValue, long currentTime, bool stable) {
    value = newValue;

    if (!stable && value == startValue()) {
      // still honing in on stable board
      changed = false;
      status = CONFIRMED;
    } else if (value && !_downStatus()) {
      changed    = true;
      lastChange = currentTime;
      status     = UNSTABLE_DOWN;
    } else if (!value && !_upStatus()) {
      changed    = true;
      lastChange = currentTime;
      status     = UNSTABLE_UP;
    }
  }

  void verifyStatus(int upCount, int downCount, int currentPlayer) {
    if (!changed) return;

    if (value) {
      // replacing piece || down w/o a companion up
      if (occupiedBy == currentPlayer || upCount == 0) {
        status = CONFIRMED;
        changed = false;
      }
    }

    // just passing by
    if (!value && occupiedBy == EMPTY) {
      status = CONFIRMED;
      changed = false;
    }
  }

  void checkStability(long currentTime) {
    if (_unstable() && _changeIsOld(currentTime)) {
      if (status == UNSTABLE_UP) {
        status = STABLE_UP;
      } else if (status == UNSTABLE_DOWN) {
        status = STABLE_DOWN;
      }
    }
  }

  void confirmState(int player) {
    if (status == STABLE_UP) {
      occupiedBy = EMPTY;
    } else if (status == STABLE_DOWN) {
      occupiedBy = player;
    }

    status = CONFIRMED;
    changed = false;
  }

  bool startValue() {
    return occupiedBy != EMPTY;
  }

private:
  bool _unstable() {
    return status == UNSTABLE_UP || status == UNSTABLE_DOWN;
  }

  bool _changeIsOld(long currentTime) {
    return currentTime - lastChange > CONFIRM_TIME;
  }

  bool _downStatus() {
    return status == UNSTABLE_DOWN || status == STABLE_DOWN;
  }

  bool _upStatus() {
    return status == UNSTABLE_UP || status == STABLE_UP;
  }
};
