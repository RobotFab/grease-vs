// snippet: active_mode_112
// tags: active, serial, command, response
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Active sketch: RAMP_UP and RAMP_DOWN smoothly change PWM on pin 5 at 50 units/second using millis
// prompts: active, sketch, ramp_up, ramp_down, smoothly, change
// verified: compile-only
// source: kd-generated (id 112)

int currentSpeed = 0;
int targetSpeed = 0;

unsigned long prevRampMs = 0;   // separate timers for ramp and report
unsigned long prevReportMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  Serial.println("Send RAMP_UP, RAMP_DOWN, or SET <0-255>");
}

void loop() {
  unsigned long now = millis();

  // Handle commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "RAMP_UP") {
      targetSpeed = 255;
    } else if (command == "RAMP_DOWN") {
      targetSpeed = 0;
    } else if (command.startsWith("SET")) {
      int spaceIdx = command.indexOf(' ');
      if (spaceIdx > 0) {
        int val = command.substring(spaceIdx + 1).toInt();
        targetSpeed = constrain(val, 0, 255);
      }
    }
  }

  // Smoothly ramp to target speed (50 units/sec = 20ms per unit)
  if (now - prevRampMs >= 20) {
    prevRampMs = now;
    if (currentSpeed < targetSpeed) {
      currentSpeed++;
    } else if (currentSpeed > targetSpeed) {
      currentSpeed--;
    }
    analogWrite(5, currentSpeed);
  }

  // Report current speed every 100ms (separate timer)
  if (now - prevReportMs >= 100) {
    prevReportMs = now;
    Serial.print("Current Speed: "); Serial.println(currentSpeed);
  }
}
