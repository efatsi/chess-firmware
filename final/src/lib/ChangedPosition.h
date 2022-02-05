class ChangedPosition {
public:
  String position;
  int    wasOccupiedBy;

  ChangedPosition() {
    // Set default values to block out space in memory
    position      = "--";
    wasOccupiedBy = 1;
  }

  ChangedPosition(String _position, int _wasOccupiedBy) {
    position      = _position;
    wasOccupiedBy = _wasOccupiedBy;
  }
};
