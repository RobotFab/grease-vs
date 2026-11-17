// snippet: openmv_rpc_line_follow
// tags: openmv, camera, rpc, line, follow, robot, centroid, angle, uart, vision, motor
// boards: mega
// verified: compile-only
// library: openmv-arduino-rpc
// description: Arduino requests line position from OpenMV cam via UART RPC for line-following.
//   OpenMV returns the line's centroid x (0-319) and angle (-90 to +90 degrees).
//   Arduino computes steering error (cx offset from center) and angle correction,
//   then prints both — wire to a motor controller to drive the robot.
//   Works best with a white/yellow line on a dark floor under the robot.
//
//   OpenMV MicroPython side (save as main.py on the cam):
//     import rpc, sensor, struct, math
//     sensor.reset(); sensor.set_pixformat(sensor.GRAYSCALE); sensor.set_framesize(sensor.QVGA)
//     sensor.skip_frames(time=2000)
//     interface = rpc.rpc_uart_slave(baudrate=115200)
//     def line_follow(data):
//         img = sensor.snapshot()
//         img = img.crop(roi=(0, img.height()//2, img.width(), img.height()//2))  # bottom half
//         lines = img.get_regression([(200, 255)], robust=True)  # white line
//         if not lines: return struct.pack("<hhh", -1, 0, 0)
//         l = lines  # get_regression returns one result
//         return struct.pack("<hhh", int(l.x1()+l.x2())//2,
//                            int(math.degrees(math.atan2(l.y2()-l.y1(), l.x2()-l.x1()))), l.magnitude())
//     interface.register_callback(line_follow)
//     interface.loop()
// prompts: OpenMV line following Arduino, RPC line detect camera robot, OpenMV line centroid angle UART, camera line follow Arduino serial, openmv-arduino-rpc line sensor, line robot OpenMV Arduino motor, vision line follow centroid angle, OpenMV get_regression Arduino
// exact-include: openmvrpc.h

#include <openmvrpc.h>

// rpc_hardware_serial1_uart_master → Serial1 (Mega TX1/RX1 or Leonardo)
openmv::rpc_scratch_buffer<256> scratch_buffer;
openmv::rpc_hardware_serial1_uart_master interface(115200);

struct __attribute__((packed)) LineResult {
  int16_t cx;        // centroid x (0-319); -1 = no line detected
  int16_t angle;     // degrees from horizontal (-90 to +90)
  int16_t magnitude;
};

const int FRAME_CX = 160;  // center of 320-wide frame

void setup() {
  Serial.begin(115200);
  interface.begin();
  Serial.println("Line follower ready");
}

void loop() {
  LineResult line = {};
  bool ok = interface.call_no_args(F("line_follow"), &line, sizeof(line));

  if (!ok) {
    Serial.println("RPC timeout");
  } else if (line.cx == -1) {
    Serial.println("No line");
  } else {
    int steer = line.cx - FRAME_CX;  // positive = line is right of center
    Serial.print("cx:"); Serial.print(line.cx);
    Serial.print(" angle:"); Serial.print(line.angle);
    Serial.print(" steer:"); Serial.print(steer);
    Serial.print(" mag:"); Serial.println(line.magnitude);
  }

  delay(50);  // 20 Hz
}
