// snippet: openmv_rpc_color_track
// tags: openmv, camera, rpc, color, blob, tracking, centroid, uart, vision, pan, tilt, servo
// boards: mega
// verified: compile-only
// library: openmv-arduino-rpc
// description: Arduino requests color blob centroid from OpenMV cam via UART RPC.
//   Result: cx, cy (centroid), area (pixels). Use cx to steer a robot or drive a pan-tilt servo.
//   The Arduino sends a LAB color threshold over RPC; the cam tracks that color in the frame.
//   cx=160 means centered in a 320-wide frame — offset from 160 gives steering error.
//
//   OpenMV MicroPython side (save as main.py on the cam):
//     import rpc, sensor, struct
//     sensor.reset(); sensor.set_pixformat(sensor.RGB565); sensor.set_framesize(sensor.QVGA)
//     sensor.skip_frames(time=2000)
//     interface = rpc.rpc_uart_slave(baudrate=115200)
//     def color_track(data):
//         # LAB thresholds packed as 6 int16: lmin,lmax,amin,amax,bmin,bmax
//         t = struct.unpack("<hhhhhh", data)
//         img = sensor.snapshot()
//         blobs = img.find_blobs([t], pixels_threshold=200, area_threshold=200, merge=True)
//         if not blobs: return struct.pack("<hhh", -1, -1, 0)
//         b = max(blobs, key=lambda x: x.pixels())
//         return struct.pack("<hhh", b.cx(), b.cy(), b.pixels())
//     interface.register_callback(color_track)
//     interface.loop()
// prompts: OpenMV color tracking Arduino, RPC blob centroid camera, OpenMV track color robot, color blob centroid serial RPC, OpenMV Arduino pan tilt color, camera color tracking UART, openmv-arduino-rpc blob tracking, follow color OpenMV Arduino
// exact-include: openmvrpc.h

#include <openmvrpc.h>

// rpc_hardware_serial1_uart_master → Serial1 (Mega TX1/RX1 or Leonardo)
openmv::rpc_scratch_buffer<256> scratch_buffer;
openmv::rpc_hardware_serial1_uart_master interface(115200);

// LAB thresholds for red — adjust with OpenMV IDE color picker.
// int16_t matches Python struct.unpack("<hhhhhh") — 6 signed shorts = 12 bytes.
// Using int8_t here would send 6 bytes but Python expects 12, corrupting the threshold.
int16_t color_thresholds[6] = { 30, 100, 15, 127, 15, 127 };

// Field order MUST match MicroPython struct.pack("<hhh", cx, cy, pixels).
struct __attribute__((packed)) BlobResult {
  uint16_t cx, cy, pixels;
};

const int FRAME_W = 320;

void setup() {
  Serial.begin(115200);
  interface.begin();
  Serial.println("Color tracker ready");
}

void loop() {
  BlobResult result = {};
  bool ok = interface.call(F("color_detection"),
                           color_thresholds, sizeof(color_thresholds),
                           &result, sizeof(result));

  if (!ok) {
    Serial.println("RPC timeout");
  } else {
    int error = result.cx - (FRAME_W / 2);  // negative = target left of center
    Serial.print("cx:"); Serial.print(result.cx);
    Serial.print(" cy:"); Serial.print(result.cy);
    Serial.print(" err:"); Serial.println(error);
  }

  delay(100);
}
