#include "Arduino.h"

#ifndef AFE4403_h
#define AFE4403_h

#define STAGES          3
#define CONTROL0       0x00

#define LED2STC        0x01
#define LED2ENDC       0x02
#define LED2LEDSTC     0x03
#define LED2LEDENDC    0x04
#define ALED2STC       0x05
#define ALED2ENDC      0x06
#define LED1STC        0x07
#define LED1ENDC       0x08
#define LED1LEDSTC     0x09
#define LED1LEDENDC    0x0A
#define ALED1STC       0x0B
#define ALED1ENDC      0x0C
#define LED2CONVST     0x0D
#define LED2CONVEND    0x0E
#define ALED2CONVST    0x0F
#define ALED2CONVEND   0x10
#define LED1CONVST     0x11
#define LED1CONVEND    0x12
#define ALED1CONVST    0x13
#define ALED1CONVEND   0x14
#define ADCRSTSTCT0    0x15
#define ADCRSTENDCT0   0x16
#define ADCRSTSTCT1    0x17
#define ADCRSTENDCT1   0x18
#define ADCRSTSTCT2    0x19
#define ADCRSTENDCT2   0x1A
#define ADCRSTSTCT3    0x1B
#define ADCRSTENDCT3   0x1C
#define PRPCOUNT       0x1D
#define CONTROL1       0x1E
#define SPARE1         0x1F
#define TIAGAIN        0x20
#define TIA_AMB_GAIN   0x21
#define LEDCNTRL       0x22
#define CONTROL2       0x23
#define SPARE2         0x24
#define SPARE3         0x25
#define SPARE4         0x26
#define RESERVED1      0x27
#define RESERVED2      0x28
#define ALARM          0x29
#define LED2VAL        0x2A
#define ALED2VAL       0x2B
#define LED1VAL        0x2C
#define ALED1VAL       0x2D
#define LED2_ALED2VAL  0x2E
#define LED1_ALED1VAL  0x2F
#define DIAG           0x30
#define CONTROL3       0x31
#define PDNCYCLESTC    0x32
#define PDNCYCLEENDC   0x33
#define sampleRate      500
#define ave 10

const  long SCALE = 33554432;
const double SOS[3][6] = {
	{ 1, 0, -1, 1, -1.9725464130456409, 0.97381705538051955 },
	{ 1, 0, -1, 1, -1.9950869055123981, 0.99513037435061724 },
	{ 1, 0, -1, 1, -1.9688341956177746, 0.96906741719379641 } };
const double s[4] = { 0.015585406745858222, 0.015585406745858222, 0.015466291403101799, 1.0 };
const int TRUNC_BITS = 25;
static unsigned int hist_count = 0;
static unsigned int decision_hist[4] = { 0, 0, 0, 0 };
static unsigned int hist_sum = 0;
static unsigned int delta_hr = 0;

class AFE4403
{
public:
	int AFE4403_STE;
	int AFE4403_PWDN;
	int AFE4403_ADCRDY;
	int AFE4403_RESET;

	int pulse;
	int oximetry;

	AFE4403();
	void register_set();
	void setLEDCurrent(uint8_t led1_current, uint8_t led2_current);
	void green_led(bool led_ste);
	void setGain(uint32_t ambdac, bool stage2, uint16_t stage2_gain, byte Cf, byte Rf);
	void pinselect(int SPI_STE, int AFE4403_PWDN, int AFE4403_RESET);
	void SPIWriteReg(byte regAddress, uint32_t regValue);
	void SPIWriteBit(byte regAddress, uint8_t bit, bool bit_high);
	void writeTimingData(byte regAddress, uint16_t timing_value);
	uint32_t SPIReadReg(byte regAddress);
	void SPIEnableRead();
	void SPIDisableRead();
	void register_set(bool led_ste);
	uint32_t readmuti(byte regAddress);
	double pulse_calculation(int count);
	void dumpreg();
	void readadc();


private:

};

#endif