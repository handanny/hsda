#include <SPI.h>
#include "AFE4403_Raw.h"

int RDY_FLAG=0,count=0;
AFE4403 AFE;

void setup() {
  
  Serial.begin(9600);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);	//CPHA = CPOL = 0    MODE = 0
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  AFE.pinselect(10,4,5); //SPI_STE , AFE4403_PWDN , AFE4403_RESET
  AFE.register_set();
  AFE.green_led(true);
  AFE.setLEDCurrent(20,20);
  AFE.setGain(0,0,0,16,0);
  //AFE.dumpreg(); 
  attachInterrupt(0,ADC_RDY_FLAG,RISING);
  
}

void loop() {
  
  if(RDY_FLAG)
  {
     RDY_FLAG=0; 
     detachInterrupt(0); 
     AFE.pulse_calculation(count);
     if(count>=10)
     count=0;
     attachInterrupt(0,ADC_RDY_FLAG,RISING);
  }
  
}

void ADC_RDY_FLAG() //interrupt Flag
{ 
  RDY_FLAG=1;
  count+=1;
}

