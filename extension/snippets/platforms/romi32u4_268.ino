// snippet: romi32u4_268
// tags: romi, romi32u4, pololu, differential-drive, motors
// boards: romi32u4
// library: Romi32U4
// description: Romi32U4: play buzzer melody then run motors
// prompts: romi32u4, buzzer, melody, motors, include, romi32u4
// verified: compile-only
// source: kd-generated (id 268)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Buzzer buzzer;
Romi32U4Motors motors;

void setup() {
  Serial.begin(9600);
  Serial.println("Melody then motors");
}

void loop() {
  static bool melodyPlayed = false;

  if (!melodyPlayed) {
    buzzer.play("!T180 L8 c e g");
    melodyPlayed = true;
  }

  if (!buzzer.isPlaying()) {
    motors.setSpeeds(200, 200);
  }
}
