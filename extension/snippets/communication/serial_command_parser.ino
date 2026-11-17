// snippet: serial_command_parser
// tags: serial, command, parse, uart, protocol, receive, string, input, buffer, sscanf
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Robust char-by-char serial command parser with fixed buffer, no String/heap
//   Reads one character at a time into a fixed char[] buffer with overflow guard.
//   Dispatches on newline. Uses sscanf or strncmp for parsing — no String class.
//   Safe on AVR (no heap fragmentation from String concatenation).
// prompts: serial command parser, parse serial commands, command dispatcher, serial protocol, parse text commands, serial menu, serial input buffer, SETPOINT command, serial control

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

#define BUF_SIZE 64
char buf[BUF_SIZE];
int  bufIdx = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready. Commands: LED_ON, LED_OFF, SPEED <value>, SETPOINT <value>");
  pinMode(LED_BUILTIN, OUTPUT);
}

void processCommand(const char* cmd) {
  float val;
  int ival;

  if (strcmp(cmd, "LED_ON") == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("OK LED_ON");
  } else if (strcmp(cmd, "LED_OFF") == 0) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("OK LED_OFF");
  } else if (sscanf(cmd, "SPEED %d", &ival) == 1) {
    Serial.print("OK SPEED=");
    Serial.println(ival);
  } else if (sscanf(cmd, "SETPOINT %f", &val) == 1) {
    Serial.print("OK SETPOINT=");
    Serial.println(val, 2);
  } else {
    Serial.print("ERR unknown: ");
    Serial.println(cmd);
  }
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (bufIdx > 0) {
        buf[bufIdx] = '\0';
        processCommand(buf);
        bufIdx = 0;
      }
    } else if (bufIdx < BUF_SIZE - 1) {
      buf[bufIdx++] = c;
    }
    // overflow: silently discard chars beyond BUF_SIZE-1
  }
}
