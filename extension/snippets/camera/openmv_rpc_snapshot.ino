// snippet: openmv_rpc_snapshot
// tags: openmv, camera, rpc, snapshot, trigger, capture, button, sd, save, uart, vision
// boards: mega
// verified: compile-only
// library: openmv-arduino-rpc
// description: Arduino triggers an OpenMV cam to capture and save a JPEG snapshot to its SD card.
//   A button on the Arduino side (D2) triggers the capture. The OpenMV cam returns the
//   filename it saved (e.g. "img_0042.jpg"). Uses UART RPC — no image pixels sent over serial.
//   Useful for data-collection robots: the cam does all storage locally, Arduino just triggers.
//
//   OpenMV MicroPython side (save as main.py on the cam):
//     import rpc, sensor, struct, uos
//     sensor.reset(); sensor.set_pixformat(sensor.RGB565); sensor.set_framesize(sensor.VGA)
//     sensor.skip_frames(time=2000)
//     interface = rpc.rpc_uart_slave(baudrate=115200)
//     count = [0]
//     def take_snapshot(data):
//         img = sensor.snapshot()
//         fname = "img_%04d.jpg" % count[0]
//         img.save("/sd/" + fname, quality=70)
//         count[0] += 1
//         return fname.encode()
//     interface.register_callback(take_snapshot)
//     interface.loop()
// prompts: OpenMV snapshot trigger Arduino, RPC capture save image camera, OpenMV button trigger JPEG save, camera capture on command Arduino, openmv-arduino-rpc snapshot SD card, trigger OpenMV save photo Arduino, take picture Arduino OpenMV UART, OpenMV data collection trigger
// exact-include: openmvrpc.h

#include <openmvrpc.h>

// rpc_hardware_serial1_uart_master → Serial1 (Mega TX1/RX1 or Leonardo)
openmv::rpc_scratch_buffer<256> scratch_buffer;
openmv::rpc_hardware_serial1_uart_master interface(115200);

const int BTN_PIN = 2;
bool lastBtn = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
  interface.begin();
  Serial.println("Snapshot trigger ready — press button on D2");
}

void loop() {
  bool btn = digitalRead(BTN_PIN);
  if (lastBtn == HIGH && btn == LOW) {
    delay(20);  // debounce
    char fname[128 + 1] = {};
    bool ok = interface.call_no_args(F("take_snapshot"), fname, sizeof(fname) - 1);

    if (!ok) {
      Serial.println("RPC timeout — cam not responding");
    } else {
      Serial.print("Saved: "); Serial.println(fname);
    }
  }
  lastBtn = btn;
}
