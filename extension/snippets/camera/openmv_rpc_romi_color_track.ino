// snippet: openmv_rpc_romi_color_track
// tags: openmv, camera, rpc, color, blob, tracking, centroid, uart, vision, romi, romi32u4, leonardo, robot, steering
// boards: romi32u4
// library: openmv-arduino-rpc
// description: Romi 32U4 requests color blob centroid from OpenMV cam via UART RPC for steering.
//   OpenMV cam runs a companion MicroPython script (shown below) that detects the largest
//   color blob matching a LAB threshold and returns its centroid (cx, cy) and pixel area.
//   The Romi uses cx offset from frame center as a steering error signal.
//
//   Hardware wiring:
//     OpenMV TX → Romi/Leonardo Serial1 RX (pin 0)
//     OpenMV RX → Romi/Leonardo Serial1 TX (pin 1)
//     OpenMV GND → Romi GND
//     OpenMV VIN → Romi 5V (OpenMV Cam H7 accepts 3.6–5V on VIN)
//
//   On Leonardo/Romi32U4 the library class is `rpc_hardware_serial_uart_master` (NO number
//   suffix). This maps to SERIAL_PORT_HARDWARE which IS Serial1 on Leonardo-based boards.
//   Do NOT use `rpc_hardware_serial1_uart_master` — that requires SERIAL_PORT_HARDWARE1
//   (only defined on Mega).
//
//   OpenMV MicroPython side (save as main.py on the cam):
//     import rpc, sensor, struct
//     sensor.reset(); sensor.set_pixformat(sensor.RGB565); sensor.set_framesize(sensor.QVGA)
//     sensor.skip_frames(time=2000)
//     interface = rpc.rpc_uart_slave(baudrate=115200)
//     def color_track(data):
//         t = struct.unpack("<hhhhhh", data)   # LAB thresholds from Arduino
//         img = sensor.snapshot()
//         blobs = img.find_blobs([t], pixels_threshold=200, area_threshold=200, merge=True)
//         if not blobs: return struct.pack("<hhH", -1, -1, 0)
//         b = max(blobs, key=lambda x: x.pixels())
//         return struct.pack("<hhH", b.cx(), b.cy(), min(b.pixels(), 65535))
//     interface.register_callback(color_track)
//     interface.loop()
//
// prompts: OpenMV Romi color tracking, RPC blob centroid Romi32U4, OpenMV track color Leonardo robot, camera color follow Romi UART, openmv-arduino-rpc Romi32U4, Romi vision color blob steering, OpenMV Leonardo Serial1 RPC, Pololu Romi camera tracking
// not-tags: esp32, nano33iot, rp2040, mega
// exact-include: openmvrpc.h

#include <openmvrpc.h>

// --- Library setup -----------------------------------------------------------
// rpc_scratch_buffer<N>: shared message buffer for ALL RPC objects.
//   N = max bytes you can send or receive in a single RPC call.
//   256 bytes is generous for struct-sized payloads.
openmv::rpc_scratch_buffer<256> scratch_buffer;

// rpc_hardware_serial_uart_master: UART-based RPC master using the board's
//   primary hardware serial port (SERIAL_PORT_HARDWARE).
//   On Leonardo/Romi32U4 this maps to Serial1 (pins 0/1).
//   Constructor arg = baud rate (must match OpenMV slave side).
openmv::rpc_hardware_serial_uart_master interface(115200);

// --- Color detection payload -------------------------------------------------
// LAB color thresholds: {L_min, L_max, A_min, A_max, B_min, B_max}
// Tune these with OpenMV IDE's color picker (Tools > Machine Vision > Threshold Editor)
// Example below tracks red objects under typical indoor lighting.
int16_t colorThreshold[6] = { 30, 100, 15, 127, 15, 127 };

// Packed result struct — must match the MicroPython struct.pack format exactly.
// "<hhH" = little-endian int16, int16, uint16
struct __attribute__((packed)) BlobResult {
  int16_t  cx;    // centroid x (0–319 in QVGA); -1 = no blob found
  int16_t  cy;    // centroid y (0–239 in QVGA)
  uint16_t area;  // blob area in pixels (capped at 65535)
};

const int16_t FRAME_CENTER_X = 160;  // half of QVGA width (320/2)

void setup() {
  Serial.begin(115200);   // USB serial for debug output
  while (!Serial && millis() < 3000);  // wait for USB enumeration (Leonardo quirk)

  // interface.begin() opens Serial1 at the baud rate specified in the constructor.
  // Must be called before any RPC transactions.
  interface.begin();

  Serial.println(F("OpenMV RPC color tracker ready"));
}

void loop() {
  BlobResult blob = {};

  // interface.call(): send data to OpenMV and receive a response.
  //   arg 1: callback name (F() = flash string) — must match interface.register_callback() name on cam
  //   arg 2: pointer to outgoing data (the LAB thresholds)
  //   arg 3: size of outgoing data in bytes
  //   arg 4: pointer to result buffer (filled by cam's return value)
  //   arg 5: size of result buffer
  //   Returns true if the cam responded within timeout (default 1000 ms).
  //
  // For calls with NO outgoing data, use interface.call_no_args(name, result_ptr, result_size).
  bool ok = interface.call(
      F("color_track"),
      colorThreshold, sizeof(colorThreshold),  // sent to cam
      &blob, sizeof(blob)                      // received from cam
  );

  if (!ok) {
    Serial.println(F("RPC timeout — check wiring & cam script"));
    delay(500);
    return;
  }

  if (blob.cx == -1) {
    Serial.println(F("No blob"));
  } else {
    // Steering error: positive = blob is right of center, negative = left
    int16_t steerError = blob.cx - FRAME_CENTER_X;
    Serial.print(F("cx:")); Serial.print(blob.cx);
    Serial.print(F(" cy:")); Serial.print(blob.cy);
    Serial.print(F(" area:")); Serial.print(blob.area);
    Serial.print(F(" steer:")); Serial.println(steerError);
    // Feed steerError into your motor PID to turn toward the blob
  }

  delay(50);  // ~20 Hz loop — matches OpenMV cam frame rate
}
