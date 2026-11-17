// snippet: robot_arm_fk_469
// tags: robot, arm, fk, forward-kinematics, servo, adafruit_gfx, adafruit_ssd1306
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit_GFX, Adafruit_SSD1306
// description: Draw the 2-DOF arm skeleton on an SSD1306 128×64 OLED: draw two lines from base→elbow→end-effecto…
// prompts: 2-dof, skeleton, ssd1306, 128×64, lines, base→elbow→end-effector
// verified: compile-only
// source: kd-generated (id 469)
// exact-include: Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int THETA1_PIN = A0;
const int THETA2_PIN = A1;
const float L1 = 30.0; // Length of first link in pixels
const float L2 = 30.0; // Length of second link in pixels

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("2DOF Arm Skeleton");
  display.display();  // SSD1306: push the framebuffer to the screen
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long INTERVAL = 50; // 20 Hz

  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    float theta1 = map(analogRead(THETA1_PIN), 0, 1023, 0, 180);
    float theta2 = map(analogRead(THETA2_PIN), 0, 1023, 0, 180);

    display.clearDisplay();  // SSD1306: clear the framebuffer (push with display())
    drawArm(theta1, theta2);
    display.display();  // SSD1306: push the framebuffer to the screen
  }
}

void drawArm(float theta1, float theta2) {
  float x1 = L1 * cos(theta1 * PI / 180);
  float y1 = L1 * sin(theta1 * PI / 180);
  float x2 = x1 + L2 * cos((theta1 + theta2) * PI / 180);
  float y2 = y1 + L2 * sin((theta1 + theta2) * PI / 180);

  display.drawLine(64, 32, 64 + x1, 32 - y1, SSD1306_WHITE);
  display.drawLine(64 + x1, 32 - y1, 64 + x2, 32 - y2, SSD1306_WHITE);
}
