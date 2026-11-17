// snippet: openmv_rpc_apriltag
// tags: openmv, camera, rpc, apriltag, fiducial, marker, id, pose, uart, vision, navigation
// boards: mega
// verified: compile-only
// library: openmv-arduino-rpc
// description: Arduino requests AprilTag detection from OpenMV cam over UART RPC.
//   Returns tag ID, family, and center position (cx, cy). Use tag ID to identify docking
//   stations, field markers, or commands in robotics applications.
//   The 36h11 family is the most robust — use it for printed markers.
//
//   OpenMV MicroPython side (save as main.py on the cam):
//     import rpc, sensor, image, struct
//     sensor.reset(); sensor.set_pixformat(sensor.GRAYSCALE); sensor.set_framesize(sensor.QQVGA)
//     sensor.set_vflip(True); sensor.skip_frames(time=2000)
//     interface = rpc.rpc_uart_slave(baudrate=115200)
//     def apriltag_detect(data):
//         img = sensor.snapshot()
//         tags = img.find_apriltags()  # default family = TAG36H11
//         if not tags: return struct.pack("<hhhh", -1, 0, 0, 0)
//         t = tags[0]  # first/closest tag
//         return struct.pack("<hhhh", t.id(), t.family(), t.cx(), t.cy())
//     interface.register_callback(apriltag_detect)
//     interface.loop()
// prompts: OpenMV AprilTag Arduino, RPC AprilTag detection camera, OpenMV detect fiducial marker, AprilTag ID UART serial RPC, OpenMV Arduino marker navigation, camera AprilTag pose Arduino, openmv-arduino-rpc apriltag, detect tag ID OpenMV robot, April tag robot navigation camera
// exact-include: openmvrpc.h

#include <openmvrpc.h>

// rpc_hardware_serial1_uart_master → Serial1 (Mega TX1/RX1 or Leonardo)
openmv::rpc_scratch_buffer<256> scratch_buffer;
openmv::rpc_hardware_serial1_uart_master interface(115200);

// Field order MUST match the MicroPython struct.pack("<hhhh", id, family, cx, cy)
// on the cam side — otherwise values land in the wrong fields.
struct __attribute__((packed)) TagResult {
  uint16_t id, family, cx, cy;
};

void setup() {
  Serial.begin(115200);
  interface.begin();
  Serial.println("AprilTag detector ready");
}

void loop() {
  TagResult result = {};
  bool ok = interface.call_no_args(F("apriltag_detect"), &result, sizeof(result));

  if (!ok) {
    Serial.println("RPC timeout");
  } else {
    Serial.print("Tag ID: "); Serial.print(result.id);
    Serial.print("  family: "); Serial.print(result.family);
    Serial.print("  cx: "); Serial.print(result.cx);
    Serial.print("  cy: "); Serial.println(result.cy);
  }

  delay(200);
}
