// snippet: i2s_max98357_amp
// tags: audio, i2s, max98357, amplifier, speaker, esp32, sound, wav
// boards: esp32
// library: none
// description: Play audio through MAX98357 I2S amplifier on ESP32 — output a sine wave tone
// prompts: MAX98357 amplifier, I2S audio output, play tone ESP32, speaker ESP32, audio amplifier, sine wave audio
// exact-include: driver/i2s.h, math.h

#include <driver/i2s.h>
#include <math.h>

#define I2S_BCLK  26
#define I2S_LRCK  25
#define I2S_DOUT  22

const int SAMPLE_RATE  = 44100;
const int FREQ_HZ      = 440;
const int AMPLITUDE    = 20000;

void setupI2S() {
  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 256,
  };
  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);

  i2s_pin_config_t pins = {
    .bck_io_num   = I2S_BCLK,
    .ws_io_num    = I2S_LRCK,
    .data_out_num = I2S_DOUT,
    .data_in_num  = I2S_PIN_NO_CHANGE,
  };
  i2s_set_pin(I2S_NUM_0, &pins);
}

void setup() {
  Serial.begin(115200);
  setupI2S();
}

void loop() {
  const int N = 256;
  int16_t buf[N * 2];
  for (int i = 0; i < N; i++) {
    float t = (float)i / SAMPLE_RATE;
    int16_t s = (int16_t)(AMPLITUDE * sin(2.0f * PI * FREQ_HZ * t));
    buf[2 * i]     = s;  // L
    buf[2 * i + 1] = s;  // R
  }
  size_t written;
  i2s_write(I2S_NUM_0, buf, sizeof(buf), &written, portMAX_DELAY);
}
