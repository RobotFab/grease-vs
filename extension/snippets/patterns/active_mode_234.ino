// snippet: active_mode_234
// tags: active, serial, command, response
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Active sketch: LED matrix control (4x4 matrix on pins 2-9)
// prompts: active, sketch, matrix, control, matrix, (rows/cols)
// verified: compile-only
// source: kd-generated (id 234)
// exact-include: Arduino.h

#include <Arduino.h>

// 4x4 LED matrix: rows on pins 2-5, cols on pins 6-9
const int ROW_PINS[4] = {2, 3, 4, 5};
const int COL_PINS[4] = {6, 7, 8, 9};

// colState[col] is a 4-bit row pattern (bit 0 = row 0, etc.)
byte colState[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(ROW_PINS[i], OUTPUT);
    pinMode(COL_PINS[i], OUTPUT);
    digitalWrite(COL_PINS[i], HIGH);  // cols active-LOW (deselect all)
  }
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();

  // Handle incoming serial commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("SET_COL")) {
      // Parse: SET_COL <col> <value>
      int col, value;
      if (sscanf(command.c_str(), "SET_COL %d %d", &col, &value) == 2) {
        if (col >= 0 && col < 4) {
          colState[col] = (byte)value;
        }
      }
    } else if (command == "CLEAR") {
      for (int i = 0; i < 4; i++) {
        colState[i] = 0;
      }
    } else if (command == "PING") {
      Serial.println("PONG");
    }
  }

  // Multiplex the matrix — one column at a time
  if (now - prevMs >= 2) {  // 2ms per column = 500Hz refresh per col
    prevMs = now;
    static int activeCol = 0;

    // Deselect all columns
    for (int c = 0; c < 4; c++) {
      digitalWrite(COL_PINS[c], HIGH);
    }

    // Set row data for the active column
    for (int r = 0; r < 4; r++) {
      digitalWrite(ROW_PINS[r], (colState[activeCol] >> r) & 0x01);
    }

    // Select the active column (active-LOW)
    digitalWrite(COL_PINS[activeCol], LOW);

    activeCol = (activeCol + 1) % 4;
  }
}
