// snippet: as7341_spectral
// tags: spectral, as7341, color, multi-channel, adafruit, i2c, wavelength, nm
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit AS7341
// description: Read 10-channel spectral data (415-940nm) from AS7341 with Adafruit library
// prompts: AS7341 spectral sensor, 10 channel color, spectral analysis, wavelength sensor, multi-channel light
// exact-include: Adafruit_AS7341.h

#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  if (!as7341.begin()) { Serial.println("AS7341 not found"); while (1); }
  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  if (!as7341.readAllChannels()) return;
  Serial.print("415nm: "); Serial.print(as7341.getChannel(AS7341_CHANNEL_415nm_F1));
  Serial.print("  445: "); Serial.print(as7341.getChannel(AS7341_CHANNEL_445nm_F2));
  Serial.print("  480: "); Serial.print(as7341.getChannel(AS7341_CHANNEL_480nm_F3));
  Serial.print("  515: "); Serial.print(as7341.getChannel(AS7341_CHANNEL_515nm_F4));
  Serial.print("  555: "); Serial.print(as7341.getChannel(AS7341_CHANNEL_555nm_F5));
  Serial.print("  590: "); Serial.print(as7341.getChannel(AS7341_CHANNEL_590nm_F6));
  Serial.print("  630: "); Serial.print(as7341.getChannel(AS7341_CHANNEL_630nm_F7));
  Serial.print("  680: "); Serial.println(as7341.getChannel(AS7341_CHANNEL_680nm_F8));
}
