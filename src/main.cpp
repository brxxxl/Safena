/*
 ADXL355-PMDZ Accelerometer Sensor Display

 Shows the output of a ADXL355 accelerometer.
 Uses the SPI library. For details on the sensor, see:
 https://wiki.analog.com/resources/eval/user-guides/eval-adicup360/hardware/adxl355

 Created 22 June 2018
 by Gabriel Vidal
 */

#include "functions.h"

void setup()
{
	Serial.begin(9600);
	SPI.begin(18, 5, 4, 17); // SCK, MISO, MOSI, CS

	// Initalize the  data ready and chip select pins:
	pinMode(CHIP_SELECT_PIN, OUTPUT);
	pinMode(16, OUTPUT);
	pinMode(15, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(2, OUTPUT);
	digitalWrite(16, LOW);
	digitalWrite(15, LOW);
	digitalWrite(4, LOW);
	digitalWrite(2, LOW);

	// Configure ADXL355:
	writeRegister(RANGE, RANGE_2G);			// 2G
	writeRegister(POWER_CTL, MEASURE_MODE); // Enable measure mode

	// Give the sensor time to set up:
	delay(100);
}

void loop()
{
	int axisAddresses[] = {XDATA1, XDATA2, XDATA3, YDATA1, YDATA2, YDATA3, ZDATA1, ZDATA2, ZDATA3};
	int axisMeasures[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	int dataSize = 9;

	// Read accelerometer data
	readMultipleData(axisAddresses, dataSize, axisMeasures);

	// Split data
	int xdata = (axisMeasures[0] >> 4) + (axisMeasures[1] << 4) + (axisMeasures[2] << 12);
	int ydata = (axisMeasures[3] >> 4) + (axisMeasures[4] << 4) + (axisMeasures[5] << 12);
	int zdata = (axisMeasures[6] >> 4) + (axisMeasures[7] << 4) + (axisMeasures[8] << 12);

	// Apply two complement
	if (xdata >= 0x80000)
	{
		xdata = ~xdata + 1;
	}
	if (ydata >= 0x80000)
	{
		ydata = ~ydata + 1;
	}
	if (zdata >= 0x80000)
	{
		zdata = ~zdata + 1;
	}

	// Print axis
	#ifndef DEBUG
	Serial.print("X=");
	Serial.print(xdata);
	Serial.print("\t");

	Serial.print("Y=");
	Serial.print(ydata);
	Serial.print("\t");

	Serial.print("Z=");
	Serial.print(zdata);
	Serial.print("\n");
	#endif
	
	#ifdef DEBUG
	Serial.print("VALS = ");
	for (int i = 0; i < dataSize; i++)
	{
		if (i == 0)
		{
			Serial.print(axisMeasures[i]);
		}
		else
		{
			Serial.print(", ");
			Serial.print(axisMeasures[i]);
		}
	}
	Serial.print("\n");
	#endif // DEBUG

	// Next data in 1000 milliseconds
	delay(100);
}