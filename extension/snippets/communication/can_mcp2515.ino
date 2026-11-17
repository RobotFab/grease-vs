// snippet: can_mcp2515
// tags: can, canbus, mcp2515, spi, obd, automotive, packet, mcp_can
// boards: TBD
// library: MCP_CAN
// description: Send and receive CAN bus frames with MCP2515 using coryjfowler/MCP_CAN_lib
// prompts: CAN bus, MCP2515, CANbus communication, OBD-II, automotive CAN, CAN frame send receive
// exact-include: mcp_can.h, SPI.h

#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN(10);  // CS on pin 10
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("CAN init OK");
  } else {
    Serial.println("CAN init failed"); while (1);
  }
  CAN.setMode(MCP_NORMAL);
  // MCP2515 INT is active-low and open-drain — needs a pull-up so digitalRead is stable
  // between interrupts (a bare INPUT floats and gives spurious "message ready" reads).
  pinMode(2, INPUT_PULLUP);  // INT pin
}

void loop() {
  unsigned long now = millis();
  // transmit
  if (now - prevMs >= 500) {
    prevMs = now;
    byte data[] = {0x01, 0x02, 0x03, 0x04};
    CAN.sendMsgBuf(0x123, 0, 4, data);
    Serial.println("TX 0x123");
  }
  // receive
  if (!digitalRead(2)) {
    long unsigned int id; byte ext, dlc, buf[8];
    CAN.readMsgBuf(&id, &ext, &dlc, buf);
    Serial.print("RX id: 0x"); Serial.print(id, HEX);
    Serial.print("  data:");
    for (int i = 0; i < dlc; i++) { Serial.print(" "); Serial.print(buf[i], HEX); }
    Serial.println();
  }
}
