#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

#define BUZZER 21
const int DELAY = 350;
#define MIN_KEYPAD 54
#define MAX_KEYPAD 61

const int RS = 29, RW = 28, EN = 26, D4 = 25, D5 = 24, D6 = 23, D7 = 22;
LiquidCrystal lcd(RS, RW, EN, D4, D5, D6, D7);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = { 54, 55, 56, 57 };
byte colPins[COLS] = { 58, 59, 60, 61 };

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char charNumbers[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

struct Time
{
  unsigned char second, minute, hour;
};
Time T = {0, 0, 0};

char currentHour[2];
char currentMinute[2];
char currentSecond[2];

int regime = 1;

ISR(TIMER3_COMPA_vect) {
  currentHour[0] = charNumbers[T.hour / 10];
  currentHour[1] = charNumbers[T.hour % 10];
  currentMinute[0] = charNumbers[T.minute / 10];
  currentMinute[1] = charNumbers[T.minute % 10];
  currentSecond[0] = charNumbers[T.second / 10];
  currentSecond[1] = charNumbers[T.second % 10];
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
  if (regime == 1) {
    lcd.setCursor(3, 0);
    lcd.print(currentHour[0]);
    lcd.print(currentHour[1]);
    lcd.print(':');
    lcd.print(currentMinute[0]);
    lcd.print(currentMinute[1]);
    lcd.print(':');
    lcd.print(currentSecond[0]);
    lcd.print(currentSecond[1]);
  }
}

void printTime(int number) {
  char result[2] = {};
  int decade = number / 10;
  int unit = number % 10;
  result[0] = charNumbers[decade];
  result[1] = charNumbers[unit];
  lcd.write(result[0]);
  lcd.write(result[1]);
}

void shortSong() {
  digitalWrite(BUZZER, HIGH);
  delay(DELAY);
  digitalWrite(BUZZER, LOW);
}

void longSong() {
  digitalWrite(BUZZER, HIGH);
  delay(2 * DELAY);
  digitalWrite(BUZZER, LOW);
}

void clearLCD() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print('0');
  lcd.print('0');
  lcd.print(':');
  lcd.print('0');
  lcd.print('0');
  lcd.print(':');
  lcd.print('0');
  lcd.print('0');
}

char alarmHour[2] = {'0', '0'};
char alarmMinute[2] = {'0', '0'};
char alarmSecond[2] = {'0', '0'};

void alarm(int go) {
  if (go == 0) {
    for (int i = 0; i < 4; i++) {
      shortSong();
      delay(DELAY);
    }
  }
}

int pushD = 0;
int go = pushD % 2;
int i = 0;
int j = 0;

char newHour[2] = {'0', '0'};
char newMinute[2] = {'0', '0'};
char newSecond[2] = {'0', '0'};

void setup() {

  noInterrupts();
  TCCR3A = 0x00;
  TCCR3B = (1 << WGM32) | (1 << CS32) | (1 << CS30);;
  TIMSK3 = (1 << OCIE3A);
  OCR3A  = 0x3D08;
  interrupts();

  DDRB = 0xFF;
  PORTB = 0;
  DDRB = 0x00;
  PORTB = 0;
  DDRC = 0x00;
  PORTC = 0;
  DDRF = 0x0F;
  PORTF = 0xF0;
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  lcd.begin(16, 2);
  for (int k = MIN_KEYPAD; k <= MAX_KEYPAD; k++)
  {
    pinMode(k, OUTPUT);
  }
  
  digitalWrite(BUZZER, LOW);
}

void loop() {

  char key = keypad.getKey();
  if (currentHour[0] == alarmHour[0] && currentHour[1] == alarmHour[1] &&
      currentMinute[0] == alarmMinute[0] && currentMinute[1] == alarmMinute[1] &&
      currentSecond[0] == alarmSecond[0] && currentSecond[1] == alarmSecond[1]) {
    alarm(go);
  }
  if (key != NO_KEY) {

    if (key == 'A')
    {
      shortSong();
      regime = 0;
      clearLCD();
      i = 0;
      j = 0;
    }

    else if (key == 'B')
    {
      shortSong();
      regime = 2;
      clearLCD();
      i = 0;
      j = 0;
    }

    else if (key == 'C')
    {
      shortSong();
      regime = 3;
      i = 0;
      j = 0;
      clearLCD();
      lcd.setCursor(3, 0);
      lcd.print(alarmHour[0]);
      lcd.print(alarmHour[1]);
      lcd.print(':');
      lcd.print(alarmMinute[0]);
      lcd.print(alarmMinute[1]);
      lcd.print(':');
      lcd.print(alarmSecond[0]);
      lcd.print(alarmSecond[1]);
      lcd.setCursor(12, 0);
      if (go == 0) {
        lcd.print('A');
      }
      else {
        lcd.print(' ');
      }
    }

    else if (key == 'D')
    {
      i = 0;
      j = 0;
      pushD++;
      go = pushD % 2;
      shortSong();
    }

    else if (key == '*')
    {
      lcd.clear();
      regime = 1;
      longSong();
      i = 0;
      j = 0;
    }

    else if (key == '#')
    {
      shortSong();
      i = 0;
      j = 0;
      int hour = (int) (newHour[0] - '0') * 10 + (newHour[1] - '0');
      int minute = (newMinute[0] - '0') * 10 + (newMinute[1] - '0');
      int second = (newSecond[0] - '0') * 10 + (newSecond[1] - '0');
      if (hour < 24 && minute < 60 && second < 60) {
        if (regime == 0) {
          T.hour = hour;
          T.minute = minute;
          T.second = second;
        }
        else if (regime == 2) {
          for (int i = 0; i < 2; i ++) {
            alarmHour[i] = newHour[i];
            alarmMinute[i] = newMinute[i];
            alarmSecond[i] = newSecond[i];
          }
        }
      }
      else {
        delay(DELAY);
        shortSong();
      }
    }
    else {
      shortSong();
      if (regime == 0) {
        lcd.setCursor(i + 3, 0);
        lcd.print(key);
        switch (i) {
          case 0:
            newHour[0] =  key;
            i++;
            break;
          case 1:
            newHour[1] = key;
            i += 2;
            break;
          case 3:
            newMinute[0] = key;
            i++;
            break;
          case 4:
            newMinute[1] = key;
            i += 2;
            break;
          case 6:
            newSecond[0] = key;
            i++;
            break;
          case 7:
            newSecond[1] = key;
            i = 0;
            break;
        }
      }
      else if (regime == 2) {
        lcd.setCursor(j + 3, 0);
        lcd.print(key);
        switch (j) {
          case 0:
            newHour[0] =  key;
            j++;
            break;
          case 1:
            newHour[1] =  key;
            j += 2;
            break;
          case 3:
            newMinute[0] =  key;
            j++;
            break;
          case 4:
            newMinute[1] =  key;
            j += 2;
            break;
          case 6:
            newSecond[0] =  key;
            j++;
            break;
          case 7:
            newSecond[1] =  key;
            j = 0;
            break;
        }
      }
    }
  }
}
