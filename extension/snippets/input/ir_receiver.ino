// snippet: ir_receiver
// tags: ir, infrared, remote, irremote, receiver, vs1838, tsop, decode
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: IRremote
// description: Decode IR remote codes using a TSOP38238 / VS1838B receiver and the IRremote library.
//   Wiring: signal pin → IR_PIN, VCC → 5V (or 3.3V on 3V boards), GND → GND.
//   Prints the decoded protocol, address, and command for each button press.
// prompts: IR remote receiver, decode IR remote control, infrared remote Arduino, IRremote library, TSOP receiver remote codes
//
// ---- CONFIGURE ----
// exact-include: IRremote.hpp
#define IR_PIN  2   // change to any digital input pin
// -------------------

#include <IRremote.hpp>

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR receiver ready — point remote and press a button");
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.print("Protocol: "); Serial.print(IrReceiver.decodedIRData.protocol);
    Serial.print("  Address: 0x"); Serial.print(IrReceiver.decodedIRData.address, HEX);
    Serial.print("  Command: 0x"); Serial.println(IrReceiver.decodedIRData.command, HEX);
    IrReceiver.resume();
  }
}
