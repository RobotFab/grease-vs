// snippet: i2s_inmp441_mic
// tags: audio, i2s, inmp441, sph0645, microphone, mic, esp32, record, sample
// boards: esp32
// library: none
// description: Capture audio from INMP441 / SPH0645 I2S MEMS microphone on ESP32
// prompts: INMP441 microphone, I2S mic, MEMS microphone ESP32, record audio ESP32, SPH0645 microphone, capture audio samples
// exact-include: driver/i2s.h

#include <driver/i2s.h>

#define I2S_SCK   14
#define I2S_WS    15
#define I2S_SD    32

void setupI2S() {
  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 128,
  };
  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);

  i2s_pin_config_t pins = {
    .bck_io_num   = I2S_SCK,
    .ws_io_num    = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num  = I2S_SD,
  };
  i2s_set_pin(I2S_NUM_0, &pins);
}

void setup() {
  Serial.begin(115200);
  setupI2S();
}

void loop() {
  int32_t raw[64];
  size_t bytesRead;
  i2s_read(I2S_NUM_0, raw, sizeof(raw), &bytesRead, portMAX_DELAY);

  int count = bytesRead / sizeof(int32_t);
  long rms = 0;
  for (int i = 0; i < count; i++) {
    long s = raw[i] >> 14;   // shift 18-bit data to 16-bit range
    rms += s * s;
  }
  rms = sqrt(rms / count);
  Serial.print("RMS: "); Serial.println(rms);
}
