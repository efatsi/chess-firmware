void printStatus() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      int index = i + (j * 8);
      Serial.print(positions[index].value);
      Serial.print(" ");
    }

    Serial.println();
  }
  Serial.println();
  delay(200);
}


void printFullStatus() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      int index = i + (j * 8);
      Position &position = positions[index];

      if (position.state != CONFIRMED) {
        Serial.print(position.position);
        Serial.print(": ");

        switch (position.state) {
          case UNSTABLE_OFF:
            switch (position.occupied) {
              case WHITE:
                Serial.println("U_off W  ");
                break;
              case BLACK:
                Serial.println("U_off B  ");
                break;
              case EMPTY:
                Serial.println("U_off _  ");
                break;
            }
            break;
          case UNSTABLE_ON:
            switch (position.occupied) {
              case WHITE:
                Serial.println("U_on  W  ");
                break;
              case BLACK:
                Serial.println("U_on  B  ");
                break;
              case EMPTY:
                Serial.println("U_on  _  ");
                break;
            }
            break;
          case UNSTABLE_OFF_ON:
            switch (position.occupied) {
              case WHITE:
                Serial.println("U_ofn W  ");
                break;
              case BLACK:
                Serial.println("U_ofn B  ");
                break;
              case EMPTY:
                Serial.println("U_ofn _  ");
                break;
            }
            break;
          case STABLE_OFF_ON:
            switch (position.occupied) {
              case WHITE:
                Serial.println("S_ofn W  ");
                break;
              case BLACK:
                Serial.println("S_ofn B  ");
                break;
              case EMPTY:
                Serial.println("S_ofn _  ");
                break;
            }
            break;
          case STABLE_OFF:
            switch (position.occupied) {
              case WHITE:
                Serial.println("S_off W  ");
                break;
              case BLACK:
                Serial.println("S_off B  ");
                break;
              case EMPTY:
                Serial.println("S_off _  ");
                break;
            }
            break;
          case STABLE_ON:
            switch (position.occupied) {
              case WHITE:
                Serial.println("S_on  W  ");
                break;
              case BLACK:
                Serial.println("S_on  B  ");
                break;
              case EMPTY:
                Serial.println("S_on  _  ");
                break;
            }
            break;
        }
      }
    }
  }
  delay(500);
}

void showPercentages() {
  for (size_t i = 0; i < TOTAL; i++) {
    Serial.print(rawReadings[i] * 100 / upperNorm);
    Serial.print("%\t");
  }
  Serial.print("Light: ");
  Serial.println(upperNorm);
}

void printOnOff(int x, int y) {
  Serial.print(readings[x][y]);
  Serial.print("\t");
  Serial.print((readings[x][y] * 100 / upperNorm));
  Serial.print("%");
  Serial.print("\t");
  if ((readings[x][y] * 100 / upperNorm) < 35) {
    Serial.print(1);
  } else {
    Serial.print(0);
  }
  Serial.print("\t");
}
