// snippet: rosserial_publisher
// tags: ros, ros1, rosserial, publisher, topic, std_msgs, float32, serial, node
// boards: avr, megaavr, nano33iot
// verified: compile-only
// library: rosserial_arduino (ros_lib)
// description: Publish analog sensor data as a ROS1 std_msgs/Float32 topic over serial.
//   Requires rosserial_python running on the PC:
//     pip install rospkg catkin_tools
//     rosrun rosserial_python serial_node.py _port:=/dev/ttyUSB0 _baud:=57600
// prompts: rosserial publisher, ROS1 sensor data Float32, Arduino ROS topic, rosserial_python, ROS1 serial publisher
// exact-include: ros.h, std_msgs/Float32.h

#include <ros.h>
#include <std_msgs/Float32.h>

ros::NodeHandle nh;

std_msgs::Float32 sensorMsg;
ros::Publisher sensorPub("arduino/sensor", &sensorMsg);

unsigned long prevMs = 0;

void setup() {
  nh.getHardware()->setBaud(57600);
  nh.initNode();
  nh.advertise(sensorPub);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= 100) {
    prevMs = now;
    sensorMsg.data = analogRead(A0) * (5.0f / 1023.0f);
    sensorPub.publish(&sensorMsg);
  }
  nh.spinOnce();
}
