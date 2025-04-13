#include <SPI.h>
#include <Arduino.h>
#include <Wire.h>

// Pins used for the connection with the sensor
const int CHIP_SELECT_PIN = 16; // GPIO16 for CS pin

// Memory register addresses:
const int XDATA3 = 0x08;
const int XDATA2 = 0x09;
const int XDATA1 = 0x0A;
const int YDATA3 = 0x0B;
const int YDATA2 = 0x0C;
const int YDATA1 = 0x0D;
const int ZDATA3 = 0x0E;
const int ZDATA2 = 0x0F;
const int ZDATA1 = 0x10;
const int RANGE = 0x2C;
const int POWER_CTL = 0x2D;

// Device values
const int RANGE_2G = 0x01;
const int RANGE_4G = 0x02;
const int RANGE_8G = 0x03;
const int MEASURE_MODE = 0x06; // Only accelerometer

// Operations
const int READ_BYTE = 0x01;
const int WRITE_BYTE = 0x00;

/*
 * Write registry in specific device address
 */
void writeRegister(byte thisRegister, byte thisValue)
{
	byte dataToSend = (thisRegister << 1) | WRITE_BYTE;
	digitalWrite(CHIP_SELECT_PIN, LOW);
	SPI.transfer(dataToSend);
	SPI.transfer(thisValue);
	digitalWrite(CHIP_SELECT_PIN, HIGH);
}

/*
 * Read registry in specific device address
 */
unsigned int readRegistry(byte thisRegister)
{
	unsigned int result = 0;
	byte dataToSend = (thisRegister << 1) | READ_BYTE;

	digitalWrite(CHIP_SELECT_PIN, LOW);
	SPI.transfer(dataToSend);
	result = SPI.transfer(0x00);
	digitalWrite(CHIP_SELECT_PIN, HIGH);
	return result;
}

/*
 * Read multiple registries
 */
void readMultipleData(int *addresses, int dataSize, int *readedData)
{
	digitalWrite(CHIP_SELECT_PIN, LOW);
	for (int i = 0; i < dataSize; i = i + 1)
	{
		byte dataToSend = (addresses[i] << 1) | READ_BYTE;
		SPI.transfer(dataToSend);
		readedData[i] = SPI.transfer(0x00);
	}
	digitalWrite(CHIP_SELECT_PIN, HIGH);
}