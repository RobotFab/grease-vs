// snippet: analog_write_pwm
// tags: analog, analogwrite, pwm, led, fade, duty, basics
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Fade LED using analogWrite() PWM — 0-255 range on PWM-capable pins (3,5,6,9,10,11)
// prompts: analogWrite PWM, fade LED, PWM output, LED brightness, duty cycle, PWM pin

const int PWM_PIN = 9;  // must be a ~ pin on Uno/Nano

int brightness = 0;
int step = 5;

void setup() {
  pinMode(PWM_PIN, OUTPUT);
}

void loop() {
  analogWrite(PWM_PIN, brightness);
  brightness += step;
  if (brightness >= 255 || brightness <= 0) step = -step;
  delay(20);
}
