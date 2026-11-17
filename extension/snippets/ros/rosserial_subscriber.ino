// snippet: rosserial_subscriber
// tags: ros, ros1, rosserial, subscriber, topic, std_msgs, bool, led, actuator, callback
// boards: avr, megaavr, nano33iot
// verified: compile-only
// library: rosserial_arduino (ros_lib)
// description: Subscribe to a ROS1 Bool topic and toggle an LED/relay from the PC.
//   Test from PC:  rostopic pub /arduino/led std_msgs/Bool "data: true"
// prompts: rosserial subscriber, ROS1 Bool topic LED, actuator from ROS1 topic, Arduino ROS subscribe, relay ROS1
// exact-include: ros.h, std_msgs/Bool.h

#include <ros.h>
#include <std_msgs/Bool.h>

ros::NodeHandle nh;

void ledCallback(const std_msgs::Bool& msg) {
  digitalWrite(LED_BUILTIN, msg.data ? HIGH : LOW);
}

ros::Subscriber<std_msgs::Bool> ledSub("arduino/led", ledCallback);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  nh.getHardware()->setBaud(57600);
  nh.initNode();
  nh.subscribe(ledSub);
}

void loop() {
  nh.spinOnce();
  delay(1);
}
