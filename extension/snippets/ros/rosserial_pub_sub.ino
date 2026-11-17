// snippet: rosserial_pub_sub
// tags: ros, ros1, rosserial, publisher, subscriber, bidirectional, encoder, cmd_vel, motor, drive
// boards: avr, megaavr, nano33iot
// verified: compile-only
// library: rosserial_arduino (ros_lib)
// description: Bidirectional ROS1 node — publishes encoder ticks, subscribes to motor speed commands.
//   Useful as the Arduino layer of a differential drive robot.
//   PC side: rosrun rosserial_python serial_node.py _port:=/dev/ttyUSB0 _baud:=57600
// prompts: rosserial publish subscribe, ROS1 encoder odometry, bidirectional rosserial, motor command ROS1 Arduino
// exact-include: ros.h, std_msgs/Int32MultiArray.h, std_msgs/Int16MultiArray.h

#include <ros.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Int16MultiArray.h>

ros::NodeHandle nh;

// ---- encoder state (updated by ISR) ----
volatile long encL = 0, encR = 0;
void isrL() { encL++; }
void isrR() { encR++; }

// ---- motor pins ----
const int ENA = 5, IN1 = 7, IN2 = 8;
const int ENB = 6, IN3 = 4, IN4 = 3;

void setMotor(int en, int in1, int in2, int spd) {
  digitalWrite(in1, spd >= 0);
  digitalWrite(in2, spd < 0);
  analogWrite(en, constrain(abs(spd), 0, 255));
}

// ---- ROS subscriber: [leftSpeed, rightSpeed]  -255..255 ----
void cmdCallback(const std_msgs::Int16MultiArray& msg) {
  if (msg.data_length >= 2) {
    setMotor(ENA, IN1, IN2, msg.data[0]);
    setMotor(ENB, IN3, IN4, msg.data[1]);
  }
}

// ---- ROS publisher: [encL, encR] ----
std_msgs::Int32MultiArray encMsg;
long encData[2];
ros::Publisher encPub("arduino/encoders", &encMsg);
ros::Subscriber<std_msgs::Int16MultiArray> cmdSub("arduino/cmd_motors", cmdCallback);

unsigned long prevMs = 0;

void setup() {
  int outPins[] = {ENA, IN1, IN2, ENB, IN3, IN4};
  for (int p : outPins) pinMode(p, OUTPUT);

  pinMode(2, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(2), isrL, RISING);
  pinMode(18, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(18), isrR, RISING);

  encMsg.data       = encData;
  encMsg.data_length = 2;

  nh.getHardware()->setBaud(57600);
  nh.initNode();
  nh.advertise(encPub);
  nh.subscribe(cmdSub);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 50) {
    prevMs = now;
    noInterrupts(); encData[0] = encL; encData[1] = encR; interrupts();
    encPub.publish(&encMsg);
  }
  nh.spinOnce();
}
