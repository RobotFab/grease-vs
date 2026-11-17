// snippet: motor_brushless_273
// tags: motor, brushless, esc, pwm, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Dual ESC for two brushless motors on pins 9 and 10
// prompts: brushless, motors, include, servo, declare, servo
// verified: compile-only
// source: kd-generated (id 273)
// exact-include: Servo.h

#include <Servo.h>

Servo esc1;
Servo esc2;

void armESC(Servo &esc) {
  esc.writeMicroseconds(1000);  // minimum throttle
  delay(2000);                  // wait for ESC to arm (beep sequence)
}

void setup() {
  Serial.begin(9600);
  esc1.attach(9, 1000, 2000);  // min 1000µs, max 2000µs
  esc2.attach(10, 1000, 2000); // min 1000µs, max 2000µs
  armESC(esc1);
  armESC(esc2);
  Serial.println("ESCs armed");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "STOP") {
      esc1.writeMicroseconds(1000);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
      esc2.writeMicroseconds(1000);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
      Serial.println("Throttle set to 1000");
    } else if (command.startsWith("LEFT")) {
      int throttle = command.substring(5).toInt();
      esc1.writeMicroseconds(map(throttle, 0, 100, 1000, 2000));  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
      Serial.print("LEFT throttle: ");
      Serial.println(esc1.read());
    } else if (command.startsWith("RIGHT")) {
      int throttle = command.substring(6).toInt();
      esc2.writeMicroseconds(map(throttle, 0, 100, 1000, 2000));  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
      Serial.print("RIGHT throttle: ");
      Serial.println(esc2.read());
    }
  }
}
