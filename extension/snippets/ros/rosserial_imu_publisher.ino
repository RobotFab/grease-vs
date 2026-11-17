// snippet: rosserial_imu_publisher
// tags: ros, ros1, rosserial, imu, sensor_msgs, publisher, mpu6050, quaternion, angular
// boards: avr, megaavr, nano33iot
// verified: compile-only
// library: rosserial_arduino (ros_lib), MPU6050
// description: Publish sensor_msgs/Imu over ROS1 from MPU-6050.
//   Covariance values are set to -1 (unknown) — calibrate and fill them in for fusion nodes.
//   Visualize: rviz → Add → Imu (topic: arduino/imu)
// prompts: rosserial IMU publisher, ROS1 MPU6050, sensor_msgs Imu rosserial, IMU publisher ROS Mega, rosserial IMU
// not-tags: mpu9250, lsm6ds3, lsm6dsox, adxl345, bno055, bno085, bno08x, icm20948, icm42688, lis3dh
// exact-include: ros.h, sensor_msgs/Imu.h, Wire.h, MPU6050.h

#include <ros.h>
#include <sensor_msgs/Imu.h>
#include <Wire.h>
#include <MPU6050.h>

ros::NodeHandle nh;
sensor_msgs::Imu imuMsg;
ros::Publisher imuPub("arduino/imu", &imuMsg);

MPU6050 mpu;
unsigned long prevMs = 0;

void setup() {
  Wire.begin();
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())

  // unknown covariance — mark as -1 per REP-145
  for (int i = 0; i < 9; i++) {
    imuMsg.orientation_covariance[i]         = (i == 0) ? -1 : 0;
    imuMsg.angular_velocity_covariance[i]    = (i % 4 == 0) ? 0.02 : 0;
    imuMsg.linear_acceleration_covariance[i] = (i % 4 == 0) ? 0.04 : 0;
  }
  imuMsg.header.frame_id = "imu_link";

  nh.getHardware()->setBaud(57600);
  nh.initNode();
  nh.advertise(imuPub);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) { nh.spinOnce(); return; }
  prevMs = now;

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)

  // Convert raw to SI units (MPU6050 ±2g / ±250°/s defaults)
  imuMsg.linear_acceleration.x = ax / 16384.0f * 9.81f;
  imuMsg.linear_acceleration.y = ay / 16384.0f * 9.81f;
  imuMsg.linear_acceleration.z = az / 16384.0f * 9.81f;

  imuMsg.angular_velocity.x = gx / 131.0f * (PI / 180.0f);
  imuMsg.angular_velocity.y = gy / 131.0f * (PI / 180.0f);
  imuMsg.angular_velocity.z = gz / 131.0f * (PI / 180.0f);

  // No onboard quaternion — orientation covariance[0] = -1 signals "not provided"
  imuMsg.orientation.w = 1;
  imuMsg.orientation.x = 0;
  imuMsg.orientation.y = 0;
  imuMsg.orientation.z = 0;

  imuMsg.header.stamp    = nh.now();
  imuMsg.header.seq++;
  imuPub.publish(&imuMsg);
  nh.spinOnce();
}
