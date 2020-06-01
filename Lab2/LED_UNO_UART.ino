#include <avr/io.h>
#include <avr/interrupt.h>
#define buttonPin1 2
#define buttonPin2 13
#define LED_PIN_MIN 4
#define LED_PIN_MAX 11
#define SLEEP_MS 1050

void high_to_low(int port)
{
  digitalWrite(port, HIGH);
  delay(SLEEP_MS);
  digitalWrite(port, LOW);  
}

void setup() {
  
  Serial.begin(9600);
  for(int i = LED_PIN_MIN; i ++; i < LED_PIN_MIN){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
}

void loop() {
  if (Serial.available()) {
    int inByte = Serial.read();
    if (inByte == 0xA1) {
      for(int i = LED_PIN_MIN; i++; i <= LED_PIN_MAX){
        high_to_low(i);
      }
    } 
    else if (inByte == 0xB1) {
       for(int i = LED_PIN_MAX ; i -= 2; i >= LED_PIN_MIN + 1){
         high_to_low(i);
       }
       for(int i = LED_PIN_MAX - 1; i -= 2; i >= LED_PIN_MIN){
         high_to_low(i);
       }
    }
  }
 
  if(digitalRead(buttonPin1)==LOW){
    Serial.write(0xB1);
    delay(200);
  }
  else if (digitalRead(buttonPin2)==LOW){
    Serial.write(0xB2);
    delay(200);
  }
}
