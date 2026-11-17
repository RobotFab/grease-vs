// snippet: display_oled_328
// tags: oled, ssd1306, mpu6050, pitch, roll, display, i2c, imu, adafruit_gfx
// not-tags: Adafruit_MPU6050, lsm6ds3, nano_33_iot
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306, MPU6050 (jrowberg i2cdevlib)
// description: Read pitch and roll from MPU6050 (jrowberg) and display with horizon line on SSD1306 OLED at 20 Hz.
//   MPU6050 API: mpu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz) — raw int16_t. atan2 ratio is scale-independent.
//   NOT Adafruit_MPU6050 — use MPU6050.h (jrowberg). getMotion6 is the only correct read call.
// prompts: MPU6050 OLED display pitch roll, horizon line SSD1306 MPU6050, tilt display, IMU OLED 20Hz, pitch roll display
// verified: compile-only
// source: kd-generated (id 328)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h, MPU6050.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();  // MPU6050: returns void - check connection with testConnection(), not if(!initialize())
  if (!mpu.testConnection()) { Serial.println("MPU6050 not found"); while (1); }
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { Serial.println("SSD1306 failed"); for (;;); }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 50) return;  // 20 Hz
  prevMs = now;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  // MPU6050: 6 int16_t* args (ax,ay,az,gx,gy,gz); Wire.begin() required (lib won't call it)
  float pitch = atan2((float)ay, (float)az) * 180.0 / PI;
  float roll  = atan2((float)ax, (float)az) * 180.0 / PI;

  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Pitch: "); display.print(pitch, 1); display.println(" deg");
  display.setCursor(0, 10);
  display.print("Roll:  "); display.print(roll, 1);  display.println(" deg");

  int horizonY = SCREEN_HEIGHT / 2 + (int)(pitch * (SCREEN_HEIGHT / 2) / 90.0);
  horizonY = constrain(horizonY, 0, SCREEN_HEIGHT - 1);
  display.drawLine(0, horizonY, SCREEN_WIDTH - 1, horizonY, SSD1306_WHITE);
  display.display();  // SSD1306: push the framebuffer to the screen
}
