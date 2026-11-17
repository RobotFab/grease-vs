// snippet: microros_subscriber_esp32
// tags: ros2, micro-ros, microros, esp32, subscriber, cmd_vel, twist, motor, drive, callback
// boards: TBD
// library: micro_ros_arduino
// description: Subscribe to ROS2 geometry_msgs/Twist (cmd_vel) and drive motors from ESP32.
//   Maps linear.x (-1..1) and angular.z (-1..1) to differential drive PWM.
//   Send from PC:  ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.5}}"
// prompts: micro-ROS subscriber, cmd_vel ESP32 ROS2, Twist message motor drive, ROS2 drive ESP32 microROS
// exact-include: micro_ros_arduino.h, rcl/rcl.h, rclc/rclc.h, rclc/executor.h, geometry_msgs/msg/twist.h

#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>

// Motor pins (L298N / generic H-bridge)
const int ENA = 25, IN1 = 26, IN2 = 27;
const int ENB = 32, IN3 = 33, IN4 = 14;
const int MAX_PWM = 200;

rcl_subscription_t  subscriber;
geometry_msgs__msg__Twist twistMsg;
rclc_executor_t     executor;
rclc_support_t      support;
rcl_allocator_t     allocator;
rcl_node_t          node;

#define RCCHECK(fn) { rcl_ret_t rc = fn; if (rc != RCL_RET_OK) errorLoop(); }

void errorLoop() {
  while (1) { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); delay(100); }
}

void setMotor(int en, int in1, int in2, int spd) {
  spd = constrain(spd, -MAX_PWM, MAX_PWM);
  digitalWrite(in1, spd >= 0 ? HIGH : LOW);
  digitalWrite(in2, spd >= 0 ? LOW  : HIGH);
  ledcWrite(en, abs(spd));  // arduino-esp32 3.x: write by pin, not channel
}

void twistCallback(const void* msgIn) {
  const geometry_msgs__msg__Twist* msg = (const geometry_msgs__msg__Twist*)msgIn;

  float lin = msg->linear.x;    // -1..1
  float ang = msg->angular.z;   // -1..1

  int left  = (int)((lin - ang) * MAX_PWM);
  int right = (int)((lin + ang) * MAX_PWM);
  setMotor(ENA, IN1, IN2, left);
  setMotor(ENB, IN3, IN4, right);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  int outPins[] = {IN1, IN2, IN3, IN4};
  for (int p : outPins) pinMode(p, OUTPUT);

  // ESP32 LEDC for PWM (arduino-esp32 3.x API: attach by pin, freq, resolution —
  // ledcSetup/ledcAttachPin were removed in core 3.0).
  ledcAttach(ENA, 5000, 8);
  ledcAttach(ENB, 5000, 8);

  set_microros_transports();

  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "motor_node", "", &support));

  RCCHECK(rclc_subscription_init_default(
    &subscriber, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
    "cmd_vel"));

  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(
    &executor, &subscriber, &twistMsg, twistCallback, ON_NEW_DATA));
}

void loop() {
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
  delay(1);
}
