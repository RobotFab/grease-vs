// snippet: esp32_dual_core
// tags: esp32, freertos, task, core, dual-core, parallel, rtos, multitask
// boards: esp32
// library: none
// description: Run tasks on both ESP32 cores using FreeRTOS — sensor read on core 0, loop on core 1
// prompts: ESP32 dual core FreeRTOS, run tasks both cores, parallel tasks ESP32, core 0 core 1, RTOS xTaskCreatePinnedToCore

TaskHandle_t sensorTask;

void readSensors(void* param) {
  for (;;) {
    float v = analogRead(34) * (3.3f / 4095.0f);
    Serial.print("Core0 sensor: "); Serial.println(v, 3);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    readSensors,
    "SensorTask",
    4096,       // stack size (bytes)
    NULL,       // parameter
    1,          // priority
    &sensorTask,
    0           // core 0
  );
}

void loop() {
  // core 1 (default Arduino loop core)
  Serial.println("Core1 loop");
  delay(500);
}
