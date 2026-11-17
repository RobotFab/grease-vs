// snippet: microros_imu_publisher
// tags: ros2, micro-ros, microros, esp32, imu, sensor_msgs, publisher, mpu6050, angular, linear
// boards: TBD
// library: micro_ros_arduino, MPU6050
// description: Publish sensor_msgs/Imu over ROS2 from ESP32 + MPU-6050 using micro-ROS.
//   header.frame_id = "imu_link" — add a static TF in your robot URDF.
//   Visualize: rviz2 → Add → Imu → topic: /arduino/imu
// prompts: micro-ROS IMU publisher, ROS2 ESP32 IMU, sensor_msgs Imu microROS, microROS MPU6050, imu_link ROS2 topic
// not-tags: mpu9250, lsm6ds3, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh
// exact-include: micro_ros_arduino.h, rcl/rcl.h, rclc/rclc.h, rclc/executor.h, sensor_msgs/msg/imu.h, Wire.h, MPU6050.h

#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <sensor_msgs/msg/imu.h>
#include <Wire.h>
#include <MPU6050.h>

rcl_publisher_t    publisher;
sensor_msgs__msg__Imu imuMsg;
rclc_executor_t    executor;
rclc_support_t     support;
rcl_allocator_t    allocator;
rcl_node_t         node;
rcl_timer_t        timer;

MPU6050 mpu;

#define RCCHECK(fn) { rcl_ret_t rc = fn; if (rc != RCL_RET_OK) errorLoop(); }

void errorLoop() {
  while (1) { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); delay(100); }
}

void timerCallback(rcl_timer_t* t, int64_t last_call_time) {
  (void)last_call_time;
  if (!t) return;

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)

  imuMsg.linear_acceleration.x = ax / 16384.0 * 9.81;
  imuMsg.linear_acceleration.y = ay / 16384.0 * 9.81;
  imuMsg.linear_acceleration.z = az / 16384.0 * 9.81;

  imuMsg.angular_velocity.x = gx / 131.0 * (PI / 180.0);
  imuMsg.angular_velocity.y = gy / 131.0 * (PI / 180.0);
  imuMsg.angular_velocity.z = gz / 131.0 * (PI / 180.0);

  // No quaternion from raw MPU6050 — signal unknown with covariance[0] = -1
  imuMsg.orientation.w = 1;
  imuMsg.orientation_covariance[0] = -1;

  int64_t now = rmw_uros_epoch_nanos();
  imuMsg.header.stamp.sec     = (int32_t)(now / 1000000000LL);
  imuMsg.header.stamp.nanosec = (uint32_t)(now % 1000000000LL);

  RCCHECK(rcl_publish(&publisher, &imuMsg, NULL));
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())

  // set frame id
  static char frame[] = "imu_link";
  imuMsg.header.frame_id.data   = frame;
  imuMsg.header.frame_id.size   = strlen(frame);
  imuMsg.header.frame_id.capacity = strlen(frame) + 1;

  // diagonal measurement covariance (rad²/s² and m²/s⁴)
  for (int i = 0; i < 9; i++) {
    imuMsg.angular_velocity_covariance[i]    = (i % 4 == 0) ? 0.02 : 0;
    imuMsg.linear_acceleration_covariance[i] = (i % 4 == 0) ? 0.04 : 0;
  }

  set_microros_transports();
  rmw_uros_sync_session(1000);  // sync ROS time

  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "imu_node", "", &support));

  RCCHECK(rclc_publisher_init_default(
    &publisher, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
    "arduino/imu"));

  RCCHECK(rclc_timer_init_default(&timer, &support,
    RCL_MS_TO_NS(20), timerCallback));  // 50 Hz

  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
}

void loop() {
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
}
