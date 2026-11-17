// snippet: scheduler_multitask
// tags: scheduler, multitask, millis, nonblocking, periodic, tasks, interval
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Cooperative task scheduler using millis() — run multiple tasks at different rates
// prompts: multitask Arduino, run multiple tasks, cooperative scheduler, different rates, task every N ms, parallel tasks, millis multitask

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

struct Task {
  unsigned long interval;
  unsigned long lastRun;
  void (*fn)();
};

void readSensors() {
  Serial.print("sensor: "); Serial.println(analogRead(A0));
}

void blinkLed() {
  static bool state = false;
  state = !state;
  digitalWrite(LED_BUILTIN, state);
}

void sendHeartbeat() {
  Serial.println("heartbeat");
}

Task tasks[] = {
  {100,  0, readSensors},
  {500,  0, blinkLed},
  {5000, 0, sendHeartbeat},
};
const int NUM_TASKS = sizeof(tasks) / sizeof(tasks[0]);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  for (int i = 0; i < NUM_TASKS; i++) {
    if (now - tasks[i].lastRun >= tasks[i].interval) {
      tasks[i].lastRun = now;
      tasks[i].fn();
    }
  }
}
