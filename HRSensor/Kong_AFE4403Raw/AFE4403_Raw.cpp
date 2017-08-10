#include <Arduino.h>
#include <AFE4403_Raw.h>
#include <SPI.h>

int32_t dataout[6] = { 0 };
unsigned long data[6] = { 0 };
unsigned int heartRate = 0;
unsigned int heartRate2 = 10;
int transitions[2] = { -1, -1 };
int voltageCodeCounter = 0;
int pulseStarted = 0;
long maxIRValue = 0;
long minIRValue = 1000000000;
long maxRedValue = 0;
long minRedValue = 1000000000;
long previousValue = 0;
long currentValue = 0;
double spO2 = 0;
unsigned int heartReport = 10;
unsigned int pulseOx = 0;
double prate = 0, rateavr = 0, ratesum =0;
extern double rate = 0;
long pval2 = 0, pval1 = 0, cval = 0, maxv = 0, minv = 0, rval = 30000, valout;
long pout[3];
long inputNum = 0, inavg = 0, y = 0, pre_y = 0, x = 0;
float prv =0, crv = 0, k = 1,m, tau = 0.032, ts = 0.001;
double rtau = 5, rts = 1000, pre_fr, fr, xr;
int indp2 = 0, indp1 = 0, indc = 0, a;
double rateary[5] = {0};

AFE4403::AFE4403() {

}

void AFE4403::pinselect(int SPI_STE, int AFE4403_PWDN, int AFE4403_RESET) {

	AFE4403_STE = SPI_STE;

	pinMode(AFE4403_PWDN, OUTPUT);
	pinMode(AFE4403_RESET, OUTPUT);
	pinMode(AFE4403_STE, OUTPUT);

	digitalWrite(AFE4403_PWDN, HIGH);
	digitalWrite(AFE4403_RESET, HIGH);

}

void AFE4403::green_led(bool led_ste) {

	if (led_ste)
		SPIWriteReg(CONTROL3, 32775);

}

void AFE4403::register_set() {
	//PRF=500Hz, duty cycle to 25%

	writeTimingData(LED2STC, 6080);
	writeTimingData(LED2ENDC, 7998);
	writeTimingData(LED2LEDSTC, 6000);
	writeTimingData(LED2LEDENDC, 7999);
	writeTimingData(ALED2STC, 80);
	writeTimingData(ALED2ENDC, 1998);
	writeTimingData(LED1STC, 2080);
	writeTimingData(LED1ENDC, 3998);
	writeTimingData(LED1LEDSTC, 2000);
	writeTimingData(LED1LEDENDC, 3999);
	writeTimingData(ALED1STC, 4080);
	writeTimingData(ALED1ENDC, 5998);
	writeTimingData(LED2CONVST, 6);
	writeTimingData(LED2CONVEND, 1999);
	writeTimingData(ALED2CONVST, 2006);
	writeTimingData(ALED2CONVEND, 3999);
	writeTimingData(LED1CONVST, 4006);
	writeTimingData(LED1CONVEND, 5999);
	writeTimingData(ALED1CONVST, 6006);
	writeTimingData(ALED1CONVEND, 7999);
	writeTimingData(ADCRSTSTCT0, 0);
	writeTimingData(ADCRSTENDCT0, 5);
	writeTimingData(ADCRSTSTCT1, 2000);
	writeTimingData(ADCRSTENDCT1, 2005);
	writeTimingData(ADCRSTSTCT2, 4000);
	writeTimingData(ADCRSTENDCT2, 4005);
	writeTimingData(ADCRSTSTCT3, 6000);
	writeTimingData(ADCRSTENDCT3, 6005);
	writeTimingData(CONTROL1, 265);
	writeTimingData(PRPCOUNT, 7999);


}

void AFE4403::setLEDCurrent(uint8_t led1_current, uint8_t led2_current) {

	uint32_t current_val = SPIReadReg(LEDCNTRL);

	current_val = (current_val >> 16) << 16;

	current_val = current_val | (led1_current << 8);
	current_val = current_val | led2_current;

	SPIWriteReg(LEDCNTRL, current_val);
	SPIWriteBit(LEDCNTRL, 16, true);
}

void AFE4403::setGain(uint32_t ambdac, bool stage2, uint16_t stage2_gain, byte Cf, byte Rf) {
	uint32_t data = 0;
	data |= ambdac << 16;

	if (stage2){
		bitWrite(data, 14, true);
		data |= stage2_gain << 8;
	}

	data |= Cf << 3;

	data |= Rf;

	SPIWriteReg(TIA_AMB_GAIN, data);
}

void AFE4403::SPIWriteReg(byte regAddress, uint32_t regValue){

	byte temp_byte = 0;

	digitalWrite(AFE4403_STE, LOW);

	SPI.transfer(regAddress);
	temp_byte = regValue >> 16;
	SPI.transfer(temp_byte);
	temp_byte = regValue >> 8;
	SPI.transfer(temp_byte);
	temp_byte = regValue;
	SPI.transfer(temp_byte);

	digitalWrite(AFE4403_STE, HIGH);
}

void AFE4403::writeTimingData(byte regAddress, uint16_t timing_value) {

	uint32_t reg_val = timing_value;
	SPIWriteReg(regAddress, reg_val);
}

void AFE4403::SPIWriteBit(byte regAddress, uint8_t bit, bool bit_high) {

	uint32_t current_val = SPIReadReg(regAddress);
	bitWrite(current_val, bit, bit_high);

	SPIWriteReg(regAddress, current_val);


}

uint32_t AFE4403::SPIReadReg(byte regAddress){
	uint32_t temp_byte = 0;
	uint32_t reg_value = 0;

	SPIEnableRead();

	digitalWrite(AFE4403_STE, LOW);


	SPI.transfer(regAddress);

	temp_byte = SPI.transfer(0x00);
	reg_value |= temp_byte << 16;

	temp_byte = SPI.transfer(0x00);
	reg_value |= temp_byte << 8;

	temp_byte = SPI.transfer(0x00);
	reg_value |= temp_byte;

	reg_value = (reg_value << 8) >> 8;
	digitalWrite(AFE4403_STE, HIGH);

	SPIDisableRead();

	return reg_value;
}

void AFE4403::SPIEnableRead() {
	SPIWriteReg(CONTROL0, 1);
}

void AFE4403::SPIDisableRead() {
	SPIWriteReg(CONTROL0, 0);
}



uint32_t AFE4403::readmuti(byte regAddress)   
{
	uint32_t temp_byte = 0;
	uint32_t reg_value = 0;

	digitalWrite(AFE4403_STE, LOW);

	SPI.transfer(regAddress);

	temp_byte = SPI.transfer(0x00);
	reg_value |= temp_byte << 16;

	temp_byte = SPI.transfer(0x00);
	reg_value |= temp_byte << 8;

	temp_byte = SPI.transfer(0x00);
	reg_value |= temp_byte;

	reg_value = (reg_value << 8) >> 8;
	digitalWrite(AFE4403_STE, HIGH);
	return reg_value;
}

void AFE4403::dumpreg() 
{
	for (int i = 0x01; i <= 0x1E; i++){
		uint32_t data = SPIReadReg(i);
		Serial.print(i, HEX);
		Serial.print("==>");
		Serial.println(data);
	}
	for (int i = 0x20; i <= 0x23; i++){
		uint32_t data = SPIReadReg(i);
		Serial.print(i, HEX);
		Serial.print("==>");
		Serial.println(data);
	}
}


void AFE4403::readadc(){

	SPIEnableRead();
	for (int j = 0; j <= 5; j++){
		data[j] = readmuti(0x2a + j);
	}
	SPIDisableRead();

	for (int i = 0; i <= 5; i++){
		long temp = data[i] << 10;
		dataout[i] += temp / 1024;
	}
}

double AFE4403::pulse_calculation(int count)
{
	voltageCodeCounter++;

	if (voltageCodeCounter > 1000)
	{
		int index = voltageCodeCounter - 1001;
		
		if(count>=10)
		{
			readadc();

			x = dataout[4];
			pre_y = y;
			y = tau/(tau + ts) * pre_y + ts/(tau + ts) * x;

			pval2 = pval1;
			pval1 = cval;
			cval = y;

			if(pval2 > pval1 && cval > pval1)
			{
				minv = pval1;
			}

			if(minv - rval > 0)
				valout = cval - (minv - rval);
			else if (minv - rval <= 0)
				valout = cval + (rval - minv);
			
			if(valout - rval < 0)
				valout = rval;

			pout[2] = pout[1];
			pout[1] = pout[0];
			pout[0] = valout;

			if(pout[1] > pout[2] && pout[1] > pout[0])
			{
				
					prv = crv;
					crv = index/10;
			
			}
			
			if(prv < crv)
				rate = 60 / (0.013 * (crv - prv + 1));
			else
				rate = 60 / (0.013 * ((300 - prv - 1) + crv + 1));
			
			//Serial.print(index);
			//Serial.print(", ");
			//Serial.println(rate);
			//Serial.print(rval/(float)10,8);
			//Serial.print("	");
			////Serial.print(inde /10);
			//Serial.print(y/(float)10,8);
			//Serial.print("	");
			//Serial.println(valout/(float)10,8);
			//Serial.println(index/100);
			//Serial.println(dataout[4]/(float)count,8);
			//Serial.println(dataout[4]/(float)10,8);
			//Serial.println();
			
			//pval2 = pval1;
			//pval1 = cval;
			//cval = dataout[1]/(float)count;

			//if(pval1 > cval && pval1 > pval2)
			//{
			////	maxcv[2] = maxcv[1];
			////	maxcv[1] = maxcv[0];
			////	maxcv[0] = pval1;

			//	if(pval1 > maxv * 0.7)
			//	{
			//		if(pval1 > maxv * 0.85)
			//		{
			//			maxv = pval1;

			//			indp2 = indp1;
			//			indp1 = indc;
			//			
			//			indc = index / 100;
			//			
			//			prv = indp1;
			//			crv = indc;
			//		}
			//	}
			//	else
			//	{
			//		prv = prv;
			//		crv = crv;
			//	}

			//	else if(maxcv[2] > maxcv[1] && maxcv[0] > maxcv[1])
			//	{
			//		maxv = pval1;
			//	
			//		//axavg = (maxcv[0] + maxcv[1] + maxcv[2]) / 3;
			//		
			//		indp2 = indp1;
			//		indp1 = indc;
			//		
			//		indc = index / 100;
			//			
			//		prv = indp2;
			//		crv = indc;
			//	}
			//	else if(maxcv[2] < maxcv[1] && maxcv[1] < maxcv[0])
			//	{
			//		maxv = pval1;
			//	
			//		//axavg = (maxcv[0] + maxcv[1] + maxcv[2]) / 3;
			//		
			//		indp2 = indp1;
			//		indp1 = indc;
			//		
			//		indc = index / 100;
			//			
			//		prv = indp1;
			//		crv = indc;
			//	}
			//	else if(maxcv[2] < maxcv[1] && maxcv[1] > maxcv[0])
			//	{
			//		maxv = pval1;
			//	
			//		//axavg = (maxcv[0] + maxcv[1] + maxcv[2]) / 3;
			//		
			//		indp2 = indp2;
			//		indp1 = indc;
			//		
			//		indc = index / 100;
			//			
			//		prv = indp1;
			//		crv = indc;
			//	}
			//}
			//
			/*if(prv < crv)
			{	
				prate = rate;
				rate = 60 / (0.1 * (crv - prv + 1));
			}
			else
			{
				prate =rate;
				rate = 60 / (0.1 * ((30 - prv - 1) + crv + 1));
			}*/

			/*if(abs(rate - prate) > 30)
				rate = prate;*/
			//
			/*if(rate != prate)
			{
				for(int i=0;i<4;i++)
				{
					rateary[4-i] = rateary[4-i-1];
				}

				rateary[0] = rate;

				for(int i=0;i<5;i++)
				{
					ratesum = rateary[i];
				}

				rateavr = ratesum/5;
			}*/

			

			/*Serial.println(ratesum);*/
			//////Serial.println(inputNum);
			////Serial.println(inavg);
				
			for(int i=0;i<=5;i++)
			{
				dataout[i] =0;
			}

		}

		if (voltageCodeCounter >= 4000)
		{
			voltageCodeCounter = 1000;
		}

	}
	return rate;
	
}
