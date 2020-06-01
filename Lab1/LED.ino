#include <avr/io.h>
#include <avr/interrupt.h>
#define buttonPin1 22
#define buttonPin2 24

void setup() {
  DDRK = 0b11111111;
  PORTK = 0;
  pinMode(buttonPin1, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(buttonPin1) == LOW){
    PORTK = 0b10000000;
    while (PORTK) {
      delay(1050);
      PORTK = PORTK >> 2;
      if (PORTK == 0b00000010) {
        delay(1050);
        PORTK = 0b01000000;
        do {
        delay(1050);
        PORTK = PORTK >> 2;
        } while (PORTK != 0b00000001);
      }
    } 
  }
}
