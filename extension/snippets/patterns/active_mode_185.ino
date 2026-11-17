// snippet: active_mode_185
// tags: active, serial, command, response, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Active sketch: SERVO_SWEEP <start> <end> <speed> executes millis-based sweep on pin 9
// prompts: active, sketch, servo_sweep, <start>, <end>, <speed>
// verified: compile-only
// source: kd-generated (id 185)
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
bool sweeping = false;
int sweepAngle = 0;
int sweepEnd = 0;
int sweepStepMs = 20;  // ms per degree step

void setup() {
  Serial.begin(115200);
  myServo.attach(9);
  myServo.write(90);
  Serial.println("Send: SERVO_SWEEP <start> <end> <speed>");
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.startsWith("SERVO_SWEEP")) {
      // Parse space-delimited: SERVO_SWEEP <start> <end> <speed>
      int firstSpace = input.indexOf(' ');
      int secondSpace = input.indexOf(' ', firstSpace + 1);
      int thirdSpace = input.indexOf(' ', secondSpace + 1);

      if (firstSpace > 0 && secondSpace > 0) {
        int startAngle = input.substring(firstSpace + 1, secondSpace).toInt();
        startAngle = constrain(startAngle, 0, 180);

        int endAngle = 0;
        if (thirdSpace > 0) {
          endAngle = input.substring(secondSpace + 1, thirdSpace).toInt();
          int speed = input.substring(thirdSpace + 1).toInt();
          if (speed > 0) sweepStepMs = 1000 / speed;  // speed in deg/sec
        } else {
          endAngle = input.substring(secondSpace + 1).toInt();
        }
        endAngle = constrain(endAngle, 0, 180);

        sweepAngle = startAngle;
        sweepEnd = endAngle;
        myServo.write(sweepAngle);
        sweeping = true;
        prevMs = now;  // reset timer so first step waits full interval
      }
    }
  }

  if (sweeping) {
    if (now - prevMs >= (unsigned long)sweepStepMs) {
      prevMs = now;

      // Step toward end angle
      if (sweepAngle < sweepEnd) {
        sweepAngle++;
      } else if (sweepAngle > sweepEnd) {
        sweepAngle--;
      }

      myServo.write(sweepAngle);
      Serial.print("ANG:"); Serial.println(sweepAngle);

      if (sweepAngle == sweepEnd) {
        sweeping = false;
      }
    }
  }
}
