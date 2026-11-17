// snippet: makeblock_mbot
// tags: mbot, makeblock, robot, dc-motor, ultrasonic, ir, buzzer, platform
// boards: avr
// library: MakeBlockDrive
// description: mBot (Makeblock) motors, ultrasonic, and buzzer using native MakeBlock library
// prompts: mBot Makeblock robot, mBot motors ultrasonic, MakeBlock library, mBot programming starter, Makeblock mBot2
// exact-include: MeUltrasonicSensor.h, MeDCMotor.h, MeBuzzer.h

#include <MeUltrasonicSensor.h>
#include <MeDCMotor.h>
#include <MeBuzzer.h>

MeDCMotor motorL(M1);
MeDCMotor motorR(M2);
MeUltrasonicSensor us(PORT_3);
MeBuzzer buzzer;

void forward(int sp)  { motorL.run(-sp); motorR.run(sp); }
void backward(int sp) { motorL.run(sp);  motorR.run(-sp); }
void stopAll()        { motorL.stop();   motorR.stop(); }

void setup() {
  Serial.begin(9600);
}

void loop() {
  float dist = us.distanceCm();
  if (dist < 15) {
    stopAll();
    buzzer.tone(440, 200);
    backward(150);
    delay(600);
    motorL.run(150); motorR.run(150); delay(400);
    stopAll();
  } else {
    forward(200);
  }
  delay(50);
}
