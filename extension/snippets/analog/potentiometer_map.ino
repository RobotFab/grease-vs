// snippet: potentiometer_map
// tags: potentiometer, analog, map, range, scale, knob, analogread
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read potentiometer and map 0-1023 ADC range to any output range using map()
// prompts: potentiometer, map range, knob control, scale ADC value, map() function, pot to servo, variable resistor

const int POT_PIN  = A0;
const int LED_PIN  = 9;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int raw   = analogRead(POT_PIN);
  int angle = map(raw, 0, 1023, 0, 180);   // re-map to servo degrees
  int pwm   = map(raw, 0, 1023, 0, 255);   // re-map to PWM brightness
  analogWrite(LED_PIN, pwm);
  Serial.print("raw: "); Serial.print(raw);
  Serial.print("  angle: "); Serial.println(angle);
  delay(50);
}
