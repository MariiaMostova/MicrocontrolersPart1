#include <stdio.h>
#include <stdlib.h>

byte CRC8(byte[] bytes)
{
	byte generator = 0x1D;
	byte crc = 0;
	if (bytes == NULL) {
		return 0;
	}
	int len = sizeof(bytes);
	for (int j = 0; j < len; j++) {
		crc ^= 0xFF;
		while (len--) {
			crc ^= bytes[j]++;
			for (int i = 0; i < 8; i++) {
				if ((crc & 0x80) != 0) {
					crc = (byte)((crc << 1) ^ 0x1D);
				}
				else {
					crc <<= 1;
				}
			}
		}
		return crc;
	}
}

int main() {
	byte[] arr = [1, 11, 111];
	cout << CRC8(arr);
}