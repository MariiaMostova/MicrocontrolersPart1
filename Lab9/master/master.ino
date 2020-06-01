#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

bool isAddress = true;
bool isCommand = false;
byte command;

void setWriteModeRS485() {
  PORTD |= 1 << PD1;
}

ISR(USART1_TX_vect){
  PORTD &= ~(1 << PD1);
}

void setup() {
  DDRB = 0XFF;
  PORTB = 0X00;

  DDRD = 0X0E;
  PORTD = 0XF1;

  DDRD |= 1 << PD1;
  PORTD &= ~(1 << PD1);

  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1);
  UCSR1B |= (1 << UCSZ12) | (1 << TXCIE1);
}

void loop() {
  if (Serial.available()) {
    byte inByte = Serial.read();
    setWriteModeRS485();
    if (isAddress) {
      UCSR1B |= (1 << TXB81);
      isAddress = false;
      isCommand = true;
    } else if (isCommand) {
      isCommand = false;
      UCSR1B &= ~(1 << TXB81);
      if (inByte == 0xB1) {
        isAddress = true;
      }
    }
    else {
      isAddress = true;
      UCSR1B &= ~(1 << TXB81);
    }
    Serial1.write(inByte);
  }
  if (Serial1.available()) {
    byte inByte1 = Serial1.read();
    Serial.write(inByte1);
  }
}
