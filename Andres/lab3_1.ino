int Led = LED_BUILTIN;
int sensorD0 = D1;
int val;

#include "pitches.h"

int melody[] = {
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};
int duration = 500;

void setup() {
  // put your setup code here, to run once:
  pinMode(Led,OUTPUT);
  pinMode(sensorD0,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  val = digitalRead(sensorD0);                                      
  if (val==HIGH)
  {
    digitalWrite(Led,LOW);
    for (int thisNote = 0; thisNote < 8; thisNote++) {
      tone(D2,melody[thisNote], duration);
      delay(1000);
    }
    delay(2000);
  }
  else
  {
    digitalWrite(Led,HIGH);
    noTone(D2);
  }
}
