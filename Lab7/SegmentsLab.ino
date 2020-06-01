#include <avr/io.h>
#include <avr/interrupt.h>
#define BUZZER 18
const int DELAY = 350;

struct Time
{
  unsigned char hour, minute, second;
};
Time T = {0, 0, 0};
Time A = {0, 1, 0};

int button[5] = {26, 27, 28, 29, 25};
int times[6] = {0, 0, 0, 0, 0, 0};
int alarm[6] = {0, 0, 0, 0, 11, 10};
int segment[6] = {0x7C, 0x7A, 0x76, 0x6E, 0x5E, 0x3E};
int digit[12]= {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0xFF};
int go = 0;
int regime;
int alarmText[5] = {0x88, 0xC7,0x88, 0x8A, 0xC8};

ISR(TIMER1_COMPA_vect) {
  if (++T.second == 60)
  {
    T.second = 0;
    if (++T.minute == 60)
    {
      T.minute = 0;
      if (++T.hour == 24)
        T.hour = 0;
    }
  }
  times[0] = T.hour / 10;
  times[1] = T.hour % 10;
  times[2] = T.minute / 10;
  times[3] = T.minute % 10;
  times[4] = T.second / 10;
  times[5] = T.second % 10;
  
  alarm[0] = A.hour / 10;
  alarm[1] = A.hour % 10;
  alarm[2] = A.minute / 10;
  alarm[3] = A.minute % 10;
}

void song(){
  digitalWrite(BUZZER, HIGH);
  delay(DELAY);
  digitalWrite(BUZZER, LOW);  
}

void timeDisplay(){
  for(int i = 0; i < 6; i++){
    PORTC = segment[i];
    if(i == 1 or i == 3 ){
      PORTB = digit[times[i]] - 0x80;
    } else{
      PORTB = digit[times[i]]; 
    }
    delay(2); 
  }
}

void alarmDisplay(){
  if(go % 2 == 0){
    alarm[5] = 10; 
  } else{
    alarm[5] = 11; 
  }
  for(int i = 0; i < 6; i++){
    PORTC = segment[i];
    if(i == 1 or i == 3 ){
      PORTB = digit[alarm[i]] - 0x80;
    } else{
      PORTB = digit[alarm[i]]; 
    }
    delay(2);
  }
}

void alarmSongText(){
  for(int i = 0; i < 3; i++){
    song(); 
    for(int j = 0; j < 5; j++){
      PORTC = segment[j];
      PORTB = alarmText[j];
      delay(2);
    }
  delay(DELAY);
  } 
}

int first = 1;
int second = 1;
int current;

void setup() {
  noInterrupts();
  TCCR1A = 0x00;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
  TIMSK1 = (1 << OCIE1A);
  OCR1A  = 0x3D08;
  interrupts();
  DDRB = 0xFF;
  PORTB = 0x00;
  DDRC = 0xFF;
  PORTC = 0x00;
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  for(int i =0; i < 5; i++){
    pinMode(button[i], INPUT_PULLUP);
    digitalWrite(button[i], LOW);
  }
}

void loop() {

  switch(regime){
    case 0:
      timeDisplay();
      break;
    case 1:
      alarmDisplay();
      break;
  }
  
  int play = 0;
  if (go % 2 == 0){
    if(times[0] == alarm[0] && times[1] == alarm[1] &&
      times[2] == alarm[2] && times[3] == alarm[3] &&
      times[4] == 0 && times[5] == 0 ){
        alarmSongText();
        regime = 0;
    }
  }
  
  if(digitalRead(button[0]) == LOW){
    song();
    if(regime == 0){
      T.hour++;
      if (T.hour < 24){
        times[0] = T.hour / 10;
        times[1] = T.hour % 10;
      } else{
        T.hour = 0;
      }
    } else if(regime == 1){
      A.hour++;
      if (A.hour < 24){
        alarm[0] = A.hour / 10;
        alarm[1] = A.hour % 10;
      } else{
        T.hour = 0;
      }
    }    
    
  } else if(digitalRead(button[1]) == LOW){
    song();
    if(regime == 0){
      T.minute++;
      if (T.minute < 60){
        times[2] = T.minute / 10;
        times[3] = T.minute % 10;
      } else{
        T.minute = 0;
      }
    } else if(regime == 1){
      A.minute++;
      if (A.minute < 60){
        alarm[2] = A.minute / 10;
        alarm[3] = A.minute % 10;
      } else{
        T.minute = 0;
      }
    }
  } else if(digitalRead(button[2]) == LOW){
    song();
    regime = 1;
        
  } else if(digitalRead(button[3]) == LOW){
    song();
    go++;
    regime = 1;
    
  } else if(digitalRead(button[4]) == LOW){
    song();
    //regime = 0;
    for(int i = 0; i < 6; i++){
      PORTC = segment[i];
      if(i == 0){
        PORTB = digit[first];
      } else if(i == 1){
        PORTB = digit[second];
      } else{
        current = first + second;
        first = second;
        second = current;
        PORTB = digit[current]; 
      }
      delay(1000);
    }
  }
}
