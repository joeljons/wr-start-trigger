// Circuit diagram: https://crcit.net/c/a3de0d11f36645c9825a6bccb83ce9a0

#include "pitches.h"

#define TRIGGER_PIN 5
#define TRRS_SPEAKER_PIN 8
#define EXTERNAL_SPEAKER_PIN 7
#define RED_PIN 4
#define YELLOW_PIN 3
#define GREEN_PIN 2
#define BUR_DEFAULT_STATE_PIN 6

#define BUR_NEED_TO_BE_CLOSED_BEFORE_REOPEN_MS 1000

#define STATE_OPEN 0
#define STATE_CLOSING 1
#define STATE_CLOSED 2

int BUR_OPEN = -1;
int BUR_CLOSED = -1;
int state = STATE_OPEN;
unsigned long burClosedLast = 0;

int melody[] = {
  NOTE_G4,
  NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5,
  NOTE_G5, NOTE_E5, NOTE_E5,
  NOTE_F5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_F5, NOTE_G5,
  NOTE_D5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_G4,
  NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5,
  NOTE_G5, NOTE_E5, NOTE_E5,
  NOTE_F5, NOTE_G5, NOTE_E5, NOTE_F5, NOTE_D5, NOTE_C5,
  NOTE_C5, 0
};

int noteDurations[] = {
  2,
  2, 1, 1, 2, 2,
  4, 3, 1,
  2, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 2, 2,
  2, 1, 1, 2, 2,
  4, 3, 1,
  1, 1, 1, 1, 3, 1,
  4
};

void teDeum() {
  for (int thisNote = 0; melody[thisNote]; thisNote++) {
    int noteDuration = noteDurations[thisNote] * 100;
    int freq = melody[thisNote];
    light(freq <= NOTE_D5, freq <= NOTE_E5 && freq != NOTE_C5, freq >= NOTE_F5 || freq == NOTE_G4);
    tone(EXTERNAL_SPEAKER_PIN, freq, noteDuration);

    delay(noteDuration);
    light(LOW, LOW, LOW);
    delay(noteDuration * 0.3);

    noTone(EXTERNAL_SPEAKER_PIN);
    if (digitalRead(TRIGGER_PIN) == BUR_CLOSED) break;
  }
}

void setup() {
  pinMode(TRRS_SPEAKER_PIN, OUTPUT);
  pinMode(EXTERNAL_SPEAKER_PIN, OUTPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  
  pinMode(BUR_DEFAULT_STATE_PIN, INPUT_PULLUP);
  BUR_OPEN = digitalRead(BUR_DEFAULT_STATE_PIN);
  BUR_CLOSED = !BUR_OPEN;

  teDeum();

  digitalWrite(RED_PIN, HIGH);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}


void loop() {
  int trigger = digitalRead(TRIGGER_PIN);
  if (trigger == BUR_CLOSED) {
    if (state == STATE_OPEN) {  //Börja stäng buren
      state = STATE_CLOSING;
      burClosedLast = millis();
      light(LOW, HIGH, LOW);
      tone(EXTERNAL_SPEAKER_PIN, NOTE_C6, 50);
      delay(50);
    } else if (state == STATE_CLOSING) {  //Fortsätt stäng buren
      if (millis() >= burClosedLast + BUR_NEED_TO_BE_CLOSED_BEFORE_REOPEN_MS) {
        state = STATE_CLOSED;
        tone(EXTERNAL_SPEAKER_PIN, NOTE_E6, 200);
        delay(200);

        light(HIGH, LOW, LOW);
        tone(EXTERNAL_SPEAKER_PIN, NOTE_G6, 400);
      }
    } else if (state == STATE_CLOSED) {  //Buren fortfarande stängd
    }
  } else {                                //BUR_OPEN
    if (state == STATE_OPEN) {            //Buren fortfarande öppen
    } else if (state == STATE_CLOSING) {  //Stängningen avbruten
      state = STATE_OPEN;
      light(LOW, LOW, HIGH);
    } else if (state == STATE_CLOSED) {  //Trigga start
      noTone(EXTERNAL_SPEAKER_PIN);
      tone(TRRS_SPEAKER_PIN, NOTE_A4);
      light(HIGH, HIGH, HIGH);
      delay(100);
      noTone(TRRS_SPEAKER_PIN);
      tone(EXTERNAL_SPEAKER_PIN, NOTE_C7, 200);
      for (int i = 0; i < 10; i++) {
        if (digitalRead(TRIGGER_PIN) == BUR_CLOSED && i > 1) break;
        light(HIGH, HIGH, HIGH);
        delay(100);
        light(LOW, LOW, LOW);
        delay(100);
      }
      light(HIGH, HIGH, HIGH);
      for (int i = 0; i < 30; i++) {
        if (digitalRead(TRIGGER_PIN) == BUR_CLOSED) break;
        delay(100);
      }
      light(LOW, LOW, HIGH);
      state = STATE_OPEN;
    }
  }
}

void light(int green, int yellow, int red) {
  digitalWrite(RED_PIN, red);
  digitalWrite(YELLOW_PIN, yellow);
  digitalWrite(GREEN_PIN, green);
}
