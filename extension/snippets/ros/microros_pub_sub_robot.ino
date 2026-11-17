// snippet: microros_pub_sub_robot
// tags: ros2, micro-ros, microros, esp32, publisher, subscriber, encoder, cmd_vel, odom, diff-drive
// boards: esp32
// library: micro_ros_arduino
// description: Full differential drive micro-ROS node — subscribes to cmd_vel (Twist),
//   publishes encoder ticks as nav_msgs/Odometry. The odometry math runs on-device.
//   This is the Arduino side of a ROS2 nav_stack robot (pair with robot_localization on PC).
// prompts: micro-ROS differential drive, ROS2 cmd_vel subscriber, odometry publisher microROS, diff drive ROS2 ESP32
// exact-include: micro_ros_arduino.h, rcl/rcl.h, rclc/rclc.h, rclc/executor.h, geometry_msgs/msg/twist.h, nav_msgs/msg/odometry.h, math.h

#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include <nav_msgs/msg/odometry.h>
#include <math.h>

// ---- CONFIGURE ----
const float WHEEL_DIAM  = 0.065f;  // metres
const float TRACK       = 0.150f;  // metres
const int   ENC_CPR     = 360;
const int   MAX_PWM     = 200;
// -------------------

// Motor pins (L298N-style)
const int ENA=25,IN1=26,IN2=27, ENB=32,IN3=33,IN4=14;

// Encoder
volatile long encL = 0, encR = 0;
void IRAM_ATTR isrL() { encL++; }
void IRAM_ATTR isrR() { encR++; }

// Odometry state
float posX = 0, posY = 0, posH = 0;
long  prevL = 0, prevR = 0;

// micro-ROS
rcl_subscription_t cmdSub;
rcl_publisher_t    odomPub;
geometry_msgs__msg__Twist twistMsg;
nav_msgs__msg__Odometry   odomMsg;
rclc_executor_t    executor;
rclc_support_t     support;
rcl_allocator_t    allocator;
rcl_node_t         node;
rcl_timer_t        timer;

#define RCCHECK(fn) { rcl_ret_t rc = fn; if (rc != RCL_RET_OK) errorLoop(); }

void errorLoop() { while (1) { delay(100); } }

void setMotor(int en, int in1, int in2, int ch, int spd) {
  spd = constrain(spd, -MAX_PWM, MAX_PWM);
  digitalWrite(in1, spd >= 0); digitalWrite(in2, spd < 0);
  ledcWrite(ch, abs(spd));
}

void cmdCallback(const void* msgIn) {
  auto* m = (const geometry_msgs__msg__Twist*)msgIn;
  int l = (int)((m->linear.x - m->angular.z) * MAX_PWM);
  int r = (int)((m->linear.x + m->angular.z) * MAX_PWM);
  setMotor(ENA,IN1,IN2,0,l);
  setMotor(ENB,IN3,IN4,1,r);
}

void odomCallback(rcl_timer_t*, int64_t) {
  noInterrupts();
  long dL = encL - prevL;  long dR = encR - prevR;
  prevL = encL;             prevR = encR;
  interrupts();

  float mpc = WHEEL_DIAM * PI / ENC_CPR;
  float distL = dL * mpc, distR = dR * mpc;
  float dist = (distL + distR) * 0.5f;
  float dh   = (distR - distL) / TRACK;

  posH += dh;
  posX += dist * cos(posH);
  posY += dist * sin(posH);

  // fill odometry message
  int64_t now = rmw_uros_epoch_nanos();
  odomMsg.header.stamp.sec     = (int32_t)(now / 1000000000LL);
  odomMsg.header.stamp.nanosec = (uint32_t)(now % 1000000000LL);
  odomMsg.pose.pose.position.x = posX;
  odomMsg.pose.pose.position.y = posY;
  // yaw → quaternion (z-axis rotation only)
  odomMsg.pose.pose.orientation.z = sin(posH * 0.5f);
  odomMsg.pose.pose.orientation.w = cos(posH * 0.5f);

  rcl_publish(&odomPub, &odomMsg, NULL);
}

void setup() {
  int out[] = {IN1,IN2,IN3,IN4}; for (int p:out) pinMode(p,OUTPUT);
  ledcSetup(0,5000,8); ledcAttachPin(ENA,0);
  ledcSetup(1,5000,8); ledcAttachPin(ENB,1);
  pinMode(2,INPUT_PULLUP);  attachInterrupt(2,  isrL, RISING);
  pinMode(13,INPUT_PULLUP); attachInterrupt(13, isrR, RISING);

  static char odomFrame[] = "odom";
  static char baseFrame[] = "base_footprint";
  odomMsg.header.frame_id.data      = odomFrame;
  odomMsg.header.frame_id.size      = 4;
  odomMsg.child_frame_id.data       = baseFrame;
  odomMsg.child_frame_id.size       = 14;

  set_microros_transports();
  rmw_uros_sync_session(1000);

  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "diff_drive_node", "", &support));

  RCCHECK(rclc_subscription_init_default(&cmdSub, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs,msg,Twist), "cmd_vel"));
  RCCHECK(rclc_publisher_init_default(&odomPub, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs,msg,Odometry), "odom"));
  RCCHECK(rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(50), odomCallback));

  RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor,&cmdSub,&twistMsg,cmdCallback,ON_NEW_DATA));
  RCCHECK(rclc_executor_add_timer(&executor,&timer));
}

void loop() {
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
}
