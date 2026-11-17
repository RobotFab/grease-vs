// snippet: voice_command_serial
// tags: voice, speech, recognition, serial, pc, robot, command, python, whisper, drive
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: none
// description: Control a robot with spoken voice commands from the PC. Python uses the
//   SpeechRecognition library (Google Web Speech API, free) to recognise keywords and
//   maps them to serial commands. Works on macOS, Windows, and Linux.
// prompts: voice command robot, speech recognition Arduino, verbal commands serial, Python voice control, spoken drive commands
//
// ---- Python companion ----
//   pip install pyserial SpeechRecognition pyaudio
//   # Linux: sudo apt install portaudio19-dev python3-pyaudio
//
//   import serial, speech_recognition as sr
//
//   ser = serial.Serial('/dev/ttyUSB0', 115200)  # Windows: 'COM3'
//   rec = sr.Recognizer()
//   mic = sr.Microphone()
//
//   COMMANDS = {
//       'forward': 'w', 'go': 'w', 'advance': 'w',
//       'backward': 's', 'back': 's', 'reverse': 's',
//       'left': 'a', 'turn left': 'a',
//       'right': 'd', 'turn right': 'd',
//       'stop': ' ', 'halt': ' ', 'freeze': ' ',
//   }
//
//   print("Listening... say 'forward', 'back', 'left', 'right', 'stop'")
//   with mic as source:
//       rec.adjust_for_ambient_noise(source, duration=1)
//   while True:
//       with mic as source:
//           audio = rec.listen(source, phrase_time_limit=2)
//       try:
//           text = rec.recognize_google(audio).lower()
//           print(f"Heard: {text}")
//           for phrase, cmd in COMMANDS.items():
//               if phrase in text:
//                   ser.write(cmd.encode())
//                   print(f"  → sent '{cmd}'")
//                   break
//       except sr.UnknownValueError: pass
//       except sr.RequestError as e: print(f"API error: {e}")
//
// ---- Offline alternative (Whisper, no internet needed) ----
//   pip install openai-whisper sounddevice numpy
//   import whisper, sounddevice as sd, numpy as np
//   model = whisper.load_model("tiny.en")   # ~39MB, fast
//   audio = sd.rec(int(3 * 16000), samplerate=16000, channels=1, dtype='float32')
//   sd.wait()
//   result = model.transcribe(audio.flatten(), fp16=False)
//   text = result["text"].lower()
// ----------------------------------------------------------

// ESP32-safe pins: GPIO 6/7/8/11 are flash-reserved and must never be used.
// The ENx pins MUST be hardware-PWM on the target board or there is no speed control at all.
// Uno/Nano/Leonardo PWM pins: 3, 5, 6, 9, 10, 11 (+13 on Leonardo). 12 is NOT a PWM pin.
const int ENA = 5,  IN1 = 14, IN2 = 15;
const int ENB = 11, IN3 = 4,  IN4 = 3;
const int SPEED = 180, TURN_SPEED = 140;

// Voice is a SPARSE command channel — one utterance every few seconds — so a short deadman
// like the WASD sketch's would stop the robot between words. Use a long watchdog instead:
// motion expires if no recognised command has arrived in this window.
const unsigned long COMMAND_TIMEOUT_MS = 8000;
unsigned long lastCommandMs = 0;
bool moving = false;

void setMotors(int l, int r) {
  l = constrain(l, -255, 255); r = constrain(r, -255, 255);
  digitalWrite(IN1, l >= 0); digitalWrite(IN2, l < 0); analogWrite(ENA, abs(l));
  digitalWrite(IN3, r >= 0); digitalWrite(IN4, r < 0); analogWrite(ENB, abs(r));
}

void setup() {
  Serial.begin(115200);
  int pins[] = {ENA, IN1, IN2, ENB, IN3, IN4};
  for (int p : pins) pinMode(p, OUTPUT);
  Serial.println("Voice command ready");
}

void loop() {
  if (moving && millis() - lastCommandMs > COMMAND_TIMEOUT_MS) {
    setMotors(0, 0);
    moving = false;
    Serial.println("watchdog — stopped");
  }

  if (!Serial.available()) return;
  char cmd = Serial.read();
  lastCommandMs = millis();

  switch (cmd) {
    case 'w': setMotors( SPEED,  SPEED);           moving = true;  Serial.println("fwd");   break;
    case 's': setMotors(-SPEED, -SPEED);           moving = true;  Serial.println("back");  break;
    case 'a': setMotors(-TURN_SPEED,  TURN_SPEED); moving = true;  Serial.println("left");  break;
    case 'd': setMotors( TURN_SPEED, -TURN_SPEED); moving = true;  Serial.println("right"); break;
    case ' ': setMotors(0, 0);                     moving = false; Serial.println("stop");  break;
  }
}
