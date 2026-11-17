// snippet: openmv_rpc_face_detect
// tags: openmv, camera, rpc, face, detection, uart, vision, haar, serial1
// boards: mega
// verified: compile-only
// library: openmv-arduino-rpc
// description: Arduino RPC master requests face detection results from an OpenMV cam over UART.
//   Returns bounding box (x, y, w, h) of the largest detected face. Prints face count and coords.
//   OpenMV cam runs the companion MicroPython script (shown in comments below).
//   Uses Serial1 (Mega TX1/RX1 or Leonardo) at 115200 baud. The RPC handshake auto-retries.
//
//   OpenMV MicroPython side (save as main.py on the cam):
//     import rpc, sensor, image, struct
//     sensor.reset(); sensor.set_pixformat(sensor.GRAYSCALE); sensor.set_framesize(sensor.QVGA)
//     sensor.skip_frames(time=2000)
//     face_cascade = image.HaarCascade("frontalface", stages=25)
//     interface = rpc.rpc_uart_slave(baudrate=115200)
//     def face_detection(data):
//         img = sensor.snapshot()
//         objs = img.find_features(face_cascade, threshold=0.5, scale=1.25)
//         if not objs: return struct.pack("<hhhhh", 0, 0, 0, 0, 0)
//         f = max(objs, key=lambda r: r[2]*r[3])
//         return struct.pack("<hhhhh", len(objs), f[0], f[1], f[2], f[3])
//     interface.register_callback(face_detection)
//     interface.loop()
// prompts: OpenMV face detection Arduino, RPC face detect camera serial, OpenMV Arduino UART face, detect face bounding box camera, OpenMV rpc_uart_slave Arduino master, camera face detection serial output, openmv-arduino-rpc face, OpenMV haar cascade face Arduino
// exact-include: openmvrpc.h

#include <openmvrpc.h>

// rpc_hardware_serial1_uart_master → Serial1 (Mega TX1/RX1 or Leonardo)
// rpc_hardware_serial_uart_master  → Serial  (blocks USB debug — avoid on Uno)
openmv::rpc_scratch_buffer<256> scratch_buffer;
openmv::rpc_hardware_serial1_uart_master interface(115200);

// Field order MUST match MicroPython struct.pack("<hhhhh", count, x, y, w, h).
// Missing the count field shifts every subsequent value into the wrong slot.
struct __attribute__((packed)) FaceResult {
  uint16_t count, x, y, w, h;
};

void setup() {
  Serial.begin(115200);
  interface.begin();
  Serial.println("OpenMV face detect ready");
}

void loop() {
  FaceResult result = {};
  bool ok = interface.call_no_args(F("face_detection"), &result, sizeof(result));

  if (!ok) {
    Serial.println("RPC timeout — cam not responding");
  } else {
    Serial.print("Largest face x:"); Serial.print(result.x);
    Serial.print(" y:"); Serial.print(result.y);
    Serial.print(" w:"); Serial.print(result.w);
    Serial.print(" h:"); Serial.println(result.h);
  }

  delay(200);
}
