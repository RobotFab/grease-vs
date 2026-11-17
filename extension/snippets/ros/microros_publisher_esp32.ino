// snippet: microros_publisher_esp32
// tags: ros2, micro-ros, microros, esp32, publisher, topic, std_msgs, float32, serial, node
// boards: TBD
// library: micro_ros_arduino
// description: Publish sensor data as a ROS2 std_msgs/Float32 topic from ESP32 using micro-ROS.
//   Requires micro-ROS agent on the PC (Docker is easiest):
//     docker run -it --rm -v /dev:/dev --privileged microros/micro-ros-agent:humble \
//       serial --dev /dev/ttyUSB0 -b 115200
//   Then: ros2 topic echo /arduino/sensor
// prompts: micro-ROS publisher ESP32, ROS2 Float32 topic, microROS sensor publish, micro-ROS agent ESP32
// exact-include: micro_ros_arduino.h, rcl/rcl.h, rclc/rclc.h, rclc/executor.h, std_msgs/msg/float32.h

#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>

rcl_publisher_t     publisher;
std_msgs__msg__Float32 msg;
rclc_executor_t     executor;
rclc_support_t      support;
rcl_allocator_t     allocator;
rcl_node_t          node;
rcl_timer_t         timer;

#define RCCHECK(fn) { rcl_ret_t rc = fn; if (rc != RCL_RET_OK) errorLoop(); }

void errorLoop() {
  while (1) { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); delay(100); }
}

void timerCallback(rcl_timer_t* t, int64_t last_call_time) {
  (void)last_call_time;
  if (t == NULL) return;
  msg.data = analogRead(34) * (3.3f / 4095.0f);
  RCCHECK(rcl_publish(&publisher, &msg, NULL));
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  set_microros_transports();  // serial over USB at 115200

  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "arduino_node", "", &support));

  RCCHECK(rclc_publisher_init_default(
    &publisher, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "arduino/sensor"));

  RCCHECK(rclc_timer_init_default(&timer, &support,
    RCL_MS_TO_NS(100), timerCallback));

  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
}

void loop() {
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
  delay(1);
}
