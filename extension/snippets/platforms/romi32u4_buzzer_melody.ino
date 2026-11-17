// snippet: romi32u4_buzzer_melody
// tags: romi, pololu, romi32u4, buzzer, melody, notes, beep, music, tone
// boards: romi32u4
// library: Romi32U4
// description: Play a melody on the Romi32U4 onboard buzzer with buzzer.play() and one MML
//   note string — "!T120 V15 L4 c e g >c2" is C4 E4 G4 C5. play() is non-blocking; poll
//   buzzer.isPlaying() to know when the melody ends. NOT playNote(262,...) — playNote()
//   takes a note NUMBER (NOTE_C(4)), a Hz value there compiles but plays garbage pitch.
// prompts: Romi buzzer melody, Romi32U4 play notes, Romi buzzer song, Romi beep tone, Romi buzzer C E G notes, play melody Romi32U4
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Buzzer buzzer;

// One MML string plays the whole melody: tempo 120, volume 15, quarter notes,
// C4 E4 G4 then C5 as a half note ('>' raises the octave).
const char MELODY[] = "!T120 V15 L4 c e g >c2";

bool announced = false;

void setup() {
  Serial.begin(9600);
  Serial.println("melody starting");
  buzzer.play(MELODY);  // non-blocking: the buzzer plays the whole string in the background
}

void loop() {
  if (!announced && !buzzer.isPlaying()) {
    announced = true;
    Serial.println("melody done");
  }
}
