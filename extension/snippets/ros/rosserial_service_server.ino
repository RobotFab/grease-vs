// snippet: rosserial_service_server
// tags: ros, ros1, rosserial, service, server, request, response, trigger, reset
// boards: avr, megaavr, nano33iot
// verified: compile-only
// library: rosserial_arduino (ros_lib)
// description: Expose a ROS1 service from Arduino — client sends a Bool request,
//   Arduino resets encoder counters and replies with the current count.
//   Call from PC: rosservice call /arduino/reset_encoders "{data: true}"
// prompts: rosserial service server, ROS1 service Arduino, service request response, Arduino ROS service trigger
// exact-include: ros.h, std_srvs/SetBool.h

#include <ros.h>
#include <std_srvs/SetBool.h>

ros::NodeHandle nh;

volatile long encL = 0, encR = 0;

void resetCallback(const std_srvs::SetBool::Request& req,
                         std_srvs::SetBool::Response& res) {
  if (req.data) {
    noInterrupts(); encL = 0; encR = 0; interrupts();
    res.success = true;
    res.message = "encoders reset";
  } else {
    res.success = false;
    res.message = "send true to reset";
  }
}

ros::ServiceServer<std_srvs::SetBool::Request,
                   std_srvs::SetBool::Response>
  resetSrv("arduino/reset_encoders", resetCallback);

void isrL() { encL++; }
void isrR() { encR++; }

void setup() {
  pinMode(2,  INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(2),  isrL, RISING);
  pinMode(3,  INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(3),  isrR, RISING);

  nh.getHardware()->setBaud(57600);
  nh.initNode();
  nh.advertiseService(resetSrv);
}

void loop() {
  nh.spinOnce();
  delay(1);
}
