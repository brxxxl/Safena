#include <SPI.h>
#include <Arduino.h>

// Define VSPI pins based on your initial output
#define VSPI_MISO 2
#define VSPI_MOSI 4
#define VSPI_SCLK 15
#define VSPI_SS 16 // Adjust this if your CS pin is different

// function prototypes;
uint8_t vspiSend_ReceiveByte(byte stuff);

// uninitalised pointers to SPI objects
SPIClass *vspi = NULL;
static const int spiClk = 1000000; // 1 MHz

// Address for the Range Register
const uint8_t RANGE = 0x2C;					 // Considered Reset value for this Register is 0x81.
const uint8_t RANGE_2G = 0x01;				 // value to set the 2G_range of the sensor
const uint8_t POWER_CTL = 0x2D;				 // Power control Register's Address.
const uint8_t MEASURE_MODE = 0x06;			 // Only accelerometer
const uint8_t ADXL_Reset_Reg_Address = 0x2F; // this Reg is only Writable. put 0x00 to this Reg onorder to reset it.
											 // Senors's reset register address

uint8_t READ_BIT = 0x80; // MSB set for read operation in some SPI implementations
uint8_t WRITE_BIT = 0x00;
uint8_t reg_address = 0x00;

// address of the X axis
const uint8_t XDATA3 = 0x0A; // High byte (assuming this order from datasheet)
const uint8_t XDATA2 = 0x09; // Middle byte
const uint8_t XDATA1 = 0x08; // Low byte

// X axis variables
int8_t xdata[3] = {0, 0, 0};
int32_t Xdata_i = 0;
float Xdata_d = 0;

char uart_buff[50];

void setup()
{
	// initialise the SPIClass attached to VSPI
	vspi = new SPIClass(VSPI);

	// initialise vspi with the defined pins
	vspi->begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS); // SCLK, MISO, MOSI, SS

	// set up slave select pin as output
	pinMode(VSPI_SS, OUTPUT); // VSPI SS

	// resetting the ADXL
	reg_address = ADXL_Reset_Reg_Address | WRITE_BIT; // Reset register address: 0x2F
	vspiSend_ReceiveByte(reg_address);
	vspiSend_ReceiveByte(0x52); // Reset value

	// Setting the ADXL Range to 2G
	reg_address = RANGE | WRITE_BIT;
	vspiSend_ReceiveByte(reg_address);
	vspiSend_ReceiveByte(RANGE_2G);

	// Setting the ADXL to the measurement mode.
	reg_address = POWER_CTL | WRITE_BIT;
	vspiSend_ReceiveByte(reg_address);
	vspiSend_ReceiveByte(MEASURE_MODE);

	Serial.begin(115200); // Using the baud rate from your initial output
}

// the loop function runs over and over again until power down or reset
void loop()
{
	// use the VSPI bus
	// Read X-axis data
	digitalWrite(VSPI_SS, LOW);
	vspi->transfer(XDATA1 | READ_BIT); // Send read command for XDATA1
	xdata[0] = vspi->transfer(0x00);   // Receive XDATA1
	vspi->transfer(XDATA2 | READ_BIT); // Send read command for XDATA2
	xdata[1] = vspi->transfer(0x00);   // Receive XDATA2
	vspi->transfer(XDATA3 | READ_BIT); // Send read command for XDATA3
	xdata[2] = vspi->transfer(0x00);   // Receive XDATA3
	digitalWrite(VSPI_SS, HIGH);

	// Combine the 8-bit readings into a 20-bit value (adjust based on datasheet!)
	// Assuming XDATA3 is the most significant byte
	Xdata_i = ((int32_t)xdata[2] << 12) | ((int32_t)xdata[1] << 4) | ((int32_t)xdata[0] >> 4);

	// Convert to g's (adjust scaling factor based on datasheet and range!)
	Xdata_d = (float)Xdata_i / 256000.0;

	sprintf(uart_buff, "X-axis data: %f g", Xdata_d);
	Serial.println(uart_buff);

	delay(1000); // Reduced delay for faster feedback
}

uint8_t vspiSend_ReceiveByte(byte stuff)
{
	uint8_t retnd_result;
	vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
	digitalWrite(VSPI_SS, LOW);
	retnd_result = vspi->transfer(stuff);
	digitalWrite(VSPI_SS, HIGH);
	vspi->endTransaction();
	return retnd_result;
}