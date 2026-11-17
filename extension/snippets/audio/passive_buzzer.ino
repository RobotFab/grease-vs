// snippet: passive_buzzer
// tags: buzzer, audio, tone, beep, sound, passive, melody, piezo
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Play tones and melodies on a passive buzzer using tone() — non-blocking with millis
// prompts: passive buzzer, play melody, tone() buzzer, beep sound, piezo buzzer, play notes, buzzer song

#define BUZZER_PIN 8

struct Note { int freq; int dur; };
const Note melody[] = {
  {262, 250}, {294, 250}, {330, 250}, {349, 250},
  {392, 500}, {392, 500}, {0, 250}
};
const int MELODY_LEN = sizeof(melody) / sizeof(melody[0]);

int noteIdx = 0;
unsigned long noteEnd = 0;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  noteEnd = millis();
}

void loop() {
  unsigned long now = millis();
  if (now >= noteEnd) {
    noTone(BUZZER_PIN);
    if (noteIdx < MELODY_LEN) {
      if (melody[noteIdx].freq > 0) tone(BUZZER_PIN, melody[noteIdx].freq);
      noteEnd = now + melody[noteIdx].dur;
      noteIdx++;
    } else {
      noteIdx = 0;
      noteEnd = now + 1000;  // pause before repeat
    }
  }
}
