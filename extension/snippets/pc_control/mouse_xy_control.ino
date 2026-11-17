// snippet: mouse_xy_control
// tags: mouse, serial, pc, robot, pan-tilt, servo, xy, cursor, control, pynput
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Move a pan-tilt servo mount by moving the mouse. PC sends "X:<dx> Y:<dy>\n"
//   packets; Arduino maps deltas to servo angles. Works equally well for robot steering.
// prompts: mouse control servo, pan tilt mouse, cursor to servo angle, PC mouse robot, pynput servo control
//
// ---- Python companion ----
//   pip install pynput pyserial
//
//   import serial
//   from pynput import mouse
//
//   ser = serial.Serial('/dev/ttyUSB0', 115200)  # Windows: 'COM3'
//   SCALE = 0.3   # sensitivity: mouse pixels → degrees
//
//   def on_move(x, y):
//       pass   # raw position — use on_move for absolute if preferred
//
//   last = [0, 0]
//   def on_delta(x, y):
//       dx = int((x - last[0]) * SCALE)
//       dy = int((y - last[1]) * SCALE)
//       last[0], last[1] = x, y
//       if dx != 0 or dy != 0:
//           ser.write(f"X:{dx} Y:{dy}\n".encode())
//
//   with mouse.Listener(on_move=on_delta) as listener:
//       print("Move mouse to control. Ctrl-C to quit.")
//       listener.join()
// -------------------------
// exact-include: Servo.h

#include <Servo.h>

Servo panServo;   // left-right
Servo tiltServo;  // up-down

int panAngle  = 90;
int tiltAngle = 90;
String inputLine = "";

void setup() {
  Serial.begin(115200);
  panServo.attach(9);
  tiltServo.attach(10);
  panServo.write(panAngle);
  tiltServo.write(tiltAngle);
  Serial.println("Mouse control ready");
}

void loop() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n') {
      // parse "X:<dx> Y:<dy>"
      int xIdx = inputLine.indexOf("X:");
      int yIdx = inputLine.indexOf("Y:");
      if (xIdx >= 0 && yIdx >= 0) {
        int dx = inputLine.substring(xIdx + 2, yIdx - 1).toInt();
        int dy = inputLine.substring(yIdx + 2).toInt();
        panAngle  = constrain(panAngle  + dx, 0, 180);
        tiltAngle = constrain(tiltAngle - dy, 0, 180);  // invert Y for natural feel
        panServo.write(panAngle);
        tiltServo.write(tiltAngle);
      }
      inputLine = "";
    } else if (c != '\r') {
      inputLine += c;
    }
  }
}
