// snippet: romi32u4_270
// tags: romi, romi32u4, pololu, differential-drive, motors
// boards: romi32u4
// library: Romi32U4
// description: Romi32U4: serial command interface for motors
// prompts: romi32u4, serial, command, interface, motors, include
// verified: compile-only
// source: kd-generated (id 270)
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;

char cmdBuf[32];
int cmdIdx = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready. Commands: DRIVE <left> <right>, STOP, BATTERY");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processCommand(cmdBuf);
      cmdIdx = 0;
      cmdBuf[0] = '\0';
    } else if (cmdIdx < 31) {
      cmdBuf[cmdIdx++] = c;
    }
  }
}

void processCommand(const char* cmd) {
  if (strncmp(cmd, "DRIVE", 5) == 0) {
    // Parse "DRIVE <left> <right>" - skip "DRIVE " (6 chars) then find next space
    const char* p = cmd + 6;  // Skip "DRIVE "
    while (*p == ' ') p++;    // Skip leading spaces
    
    // Extract left value (up to next space or null)
    char leftStr[16] = {0};
    int i = 0;
    while (*p && *p != ' ' && i < 15) {
      leftStr[i++] = *p++;
    }
    int left = atoi(leftStr);
    
    // Skip spaces to find right value
    while (*p == ' ') p++;
    int right = atoi(p);
    
    motors.setSpeeds(left, right);
  } else if (strncmp(cmd, "STOP", 4) == 0) {
    motors.setSpeeds(0, 0);
  } else if (strncmp(cmd, "BATTERY", 7) == 0) {
    uint16_t mv = readBatteryMillivolts();
    Serial.print("Battery: "); Serial.print(mv); Serial.println(" mV");
  } else {
    Serial.println("ERR unknown command");
  }
}
