// snippet: openmv_serial_esp32_color_track
// tags: openmv, camera, serial, color, blob, tracking, centroid, uart, vision, esp32, robot, steering, serial2
// boards: esp32
// library: none
// description: ESP32 receives color blob data from OpenMV cam over Serial2 (lightweight protocol).
//   The openmv-arduino-rpc library does NOT compile on ESP32 core 3.x (CAN.h dependency uses
//   removed esp_intr.h). This snippet uses a simple trigger/response protocol instead:
//     ESP32 sends 'T' (one byte) → cam captures frame, detects blob, replies with packed struct.
//   Result: centroid (cx, cy) and pixel area of the largest blob matching a LAB threshold.
//
//   Hardware wiring:
//     OpenMV TX → ESP32 GPIO16 (Serial2 RX)
//     OpenMV RX → ESP32 GPIO17 (Serial2 TX)
//     OpenMV GND → ESP32 GND
//     OpenMV VIN → ESP32 5V (or 3.3V for OpenMV Cam M7; H7 needs 3.6–5V)
//
//   Protocol (7 bytes total response):
//     Byte 0:    0xAA (sync/start marker — confirms this is a valid frame)
//     Bytes 1-2: cx as int16 little-endian (-1 if no blob)
//     Bytes 3-4: cy as int16 little-endian
//     Bytes 5-6: area as uint16 little-endian (capped at 65535)
//
//   Why not openmv-arduino-rpc?
//     The library unconditionally #includes <CAN.h> (line 13 of openmvrpc.h).
//     The sandeepmistry/CAN library uses esp_intr.h which was removed in ESP32 Arduino
//     core 3.x (replaced by esp_intr_alloc.h). Until the library is patched, direct
//     serial communication is the reliable path on ESP32.
//
//   OpenMV MicroPython side (save as main.py on the cam):
//     import sensor, struct, time
//     from pyb import UART
//     sensor.reset(); sensor.set_pixformat(sensor.RGB565); sensor.set_framesize(sensor.QVGA)
//     sensor.skip_frames(time=2000)
//     uart = UART(3, 115200)          # UART3 = OpenMV P4(TX)/P5(RX)
//     # LAB threshold for red — tune with Tools > Machine Vision > Threshold Editor
//     threshold = (30, 100, 15, 127, 15, 127)
//     SYNC = 0xAA
//     while True:
//         if uart.any():
//             cmd = uart.readchar()
//             if cmd == ord('T'):      # trigger byte from ESP32
//                 img = sensor.snapshot()
//                 blobs = img.find_blobs([threshold], pixels_threshold=200,
//                                        area_threshold=200, merge=True)
//                 if not blobs:
//                     uart.write(struct.pack("<BhhH", SYNC, -1, -1, 0))
//                 else:
//                     b = max(blobs, key=lambda x: x.pixels())
//                     uart.write(struct.pack("<BhhH", SYNC, b.cx(), b.cy(),
//                                           min(b.pixels(), 65535)))
//
// prompts: OpenMV ESP32 color tracking, ESP32 camera serial blob detect, OpenMV ESP32 Serial2 UART, OpenMV color track ESP32 no RPC, ESP32 vision color blob steering, OpenMV lightweight protocol ESP32, camera color follow ESP32 serial, ESP32 OpenMV simple UART, openmv esp32 core 3
// not-tags: nano33iot, avr, romi32u4, rp2040, openmv-arduino-rpc
// exact-include: Arduino.h

#include <Arduino.h>

// --- Serial2 pin assignment --------------------------------------------------
// ESP32 Serial2 default: RX=GPIO16, TX=GPIO17.
// Change these if your board routes them differently (e.g. ESP32-S3).
const int RXD2 = 16;
const int TXD2 = 17;

// --- Protocol constants ------------------------------------------------------
const uint8_t TRIGGER_BYTE = 'T';   // sent to cam to request one detection frame
const uint8_t SYNC_MARKER  = 0xAA;  // first byte of every valid response
const size_t  RESPONSE_LEN = 7;     // 1 sync + 2 cx + 2 cy + 2 area
const unsigned long RPC_TIMEOUT_MS = 500;  // max wait for cam response

// --- Result struct -----------------------------------------------------------
// Packed to match the MicroPython struct.pack("<BhhH", ...) format.
// We read SYNC separately, then fill this 6-byte struct.
struct __attribute__((packed)) BlobResult {
  int16_t  cx;    // centroid x (0–319 in QVGA); -1 = no blob found
  int16_t  cy;    // centroid y (0–239 in QVGA)
  uint16_t area;  // blob pixel count (capped at 65535)
};

const int16_t FRAME_CENTER_X = 160;  // half of QVGA width (320 / 2)

void setup() {
  Serial.begin(115200);   // USB debug
  delay(100);

  // Serial2.begin(baud, config, rxPin, txPin) — ESP32-specific overload.
  // SERIAL_8N1 = 8 data bits, no parity, 1 stop bit (standard).
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  Serial.println("OpenMV ESP32 color tracker ready");
}

// readResponse: waits for SYNC byte, then reads the fixed-size payload.
// Returns true if a valid frame was received within timeout.
bool readResponse(BlobResult &blob) {
  unsigned long start = millis();

  // Wait for sync byte — discard any garbage/partial frames
  while (millis() - start < RPC_TIMEOUT_MS) {
    if (Serial2.available()) {
      uint8_t b = Serial2.read();
      if (b == SYNC_MARKER) {
        // Sync found — now read exactly sizeof(BlobResult) bytes
        size_t bytesRead = 0;
        uint8_t *buf = (uint8_t *)&blob;
        while (bytesRead < sizeof(BlobResult) && millis() - start < RPC_TIMEOUT_MS) {
          if (Serial2.available()) {
            buf[bytesRead++] = Serial2.read();
          }
        }
        return (bytesRead == sizeof(BlobResult));
      }
    }
  }
  return false;  // timeout — no sync received
}

void loop() {
  // Send trigger — cam will capture one frame and respond
  Serial2.write(TRIGGER_BYTE);

  BlobResult blob = {};
  if (!readResponse(blob)) {
    Serial.println("Timeout — check wiring & cam script");
    delay(500);
    return;
  }

  if (blob.cx == -1) {
    Serial.println("No blob");
  } else {
    // Steering error: positive = blob is right of center
    int16_t steerError = blob.cx - FRAME_CENTER_X;
    Serial.print("cx:"); Serial.print(blob.cx);
    Serial.print(" cy:"); Serial.print(blob.cy);
    Serial.print(" area:"); Serial.print(blob.area);
    Serial.print(" steer:"); Serial.println(steerError);
    // Feed steerError into motor PID for pan/tilt or differential drive
  }

  delay(50);  // ~20 Hz — matches typical OpenMV QVGA frame rate
}
