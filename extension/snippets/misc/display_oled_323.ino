// snippet: display_oled_323
// tags: adafruit_gfx, adafruit_ssd1306, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306, DHT
// description: Read a DHT22 on pin 4 and show temperature on line 1 and humidity on line 2 of an SSD1306 128×64 …
// prompts: dht22, temperature, humidity, ssd1306, 128×64, update
// verified: compile-only
// source: kd-generated (id 323)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h, DHT.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
unsigned long prevMs = 0;

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 not found");
    while (1);
  }
  dht.begin();
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 2000) return;
  prevMs = now;

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read failed");
    return;
  }

  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(temp);
  display.print(" C");
  display.setCursor(0, 10);
  display.print("Hum: ");
  display.print(hum);
  display.print(" %");
  display.display();  // SSD1306: push the framebuffer to the screen
}
