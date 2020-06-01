#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

bool isAddress = true;
byte address = 0x76;
bool isCommand = false;
byte command;
char data[4][3] = {
  {'2', '7', '*'},
  {'0', '8', '*'},
  {'2', '0', '0'},
  {'1', '*', '*'}
};

void setWriteModeRS485() {
  PORTD |= 1 << PD2;
  delay(1);
}

ISR(USART_TX_vect) {
  PORTD &= ~(1 << PD2);
}

void setup() {
  delay(1000);
  DDRB = 0XFF;
  PORTB = 0X00;

  DDRD = 0b00000111;
  PORTD = 0b11111000;

  DDRC = 0X00;
  PORTB = 0XFF;

  Serial.begin(9600, SERIAL_8N1);
  UCSR0B |= (1 << UCSZ02) | (1 << TXCIE0);
  UCSR0A |= (1 << MPCM0);
}

void loop() {
  if (Serial.available()) {
    byte inByte = Serial.read();
    if (isAddress) {
      if (address == inByte) {
        isAddress = false;
        isCommand = true;
        UCSR0A &= ~(1 << MPCM0);
      }
    } else if (isCommand) {
      isCommand = false;
      if (inByte == 0xB1) {
        isAddress = true;
        UCSR0A |= (1 << MPCM0);
        setWriteModeRS485();
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 3; j++) {
            Serial.write(data[i][j]);
          }
          byte dataCRC[3] = {data[i][0], data[i][1], data[i][2]};
          Serial.write(CRC8(dataCRC));
        }
      }
    }
  }
}

byte CRC8(byte bytes[3])
{
  byte generator = 0x1D;
  byte crc = 0;
  if (bytes == NULL) {
    return 0;
  }
  int len = sizeof(bytes);
  for (int j = 0; j < len; j++) {
    crc ^= 0xFF;
    while (len--!= 0) {
      bytes[j] *= bytes[j];
      crc ^= bytes[j]++;
      for (int i = 0; i < 8; i++) {
        if ((crc & 0x80) != 0) {
          crc = (byte)((crc << 1) ^ generator);
        }
        else {
          crc <<= 1;
        }
      }
      crc &= 0xFF;
      crc ^= 0xFF;
    }
  }
  return crc;
}
