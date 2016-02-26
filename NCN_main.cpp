/*
 * NCN_main.cpp
 *
 *  Created on: 2015-7-3
 *      Author: Jake
 *      E-mail:jake.chen@nexpaq.com
 */
#include "NCN_MDK_CPP.h"
#include "msp430.h"
#include "NCN_GPIO.h"
#include "GroveColorSensor.h"
#include "soft_i2c.h"
#include "INA219.h"
#define PINS_NUMBERS	16

unsigned char GPIO_Pins[PINS_NUMBERS] = {0x00,0x00,0x00,0xFF,
										 0x00,0x00,0x00,0x00,
										 0x00,0x00,0x00,0x00,
										 0x00,0x00,0x00,0x00};//0x00:GPIO Input; 0x01: pin0~pin2:ADC Input,pin3:,pin4~pin13:GPIO Output,pin14~pin15:soft I2C; 0xFF:invalid
GroveColorSensor colorSensor;
INA219 ina219;
#define Power3v3OutInit()	do{P3DIR |= BIT0;P3OUT &=~ BIT0;}while(0)
#define Power3v3OutOpen()	do{P3OUT |= BIT0;}while(0)
#define Power3v3OutClose()	do{P3OUT &=~BIT0;}while(0)

#define PowerBatOutInit()	do{P3DIR |= BIT4;P3OUT &=~ BIT4;}while(0)
#define PowerBatOutOpen()	do{P3OUT |= BIT4;}while(0)
#define PowerBatOutClose()	do{P3OUT &=~BIT4;}while(0)

#define PowerBatInInit()	do{P3DIR |= BIT7;P3OUT &=~ BIT7;}while(0)
#define PowerBatInOpen()	do{P3OUT |= BIT7;}while(0)
#define PowerBatInClose()	do{P3OUT &=~BIT7;}while(0)
void NCN_System_init(void) {
	pinMode(0x00, INPUT);
	pinMode(0x01, INPUT);
	pinMode(0x02, INPUT);
//	pinMode(0x03, INPUT);
	pinMode(0x04, INPUT);
	pinMode(0x05, INPUT);
	pinMode(0x06, INPUT);
	pinMode(0x07, INPUT);
	pinMode(0x08, INPUT);
	pinMode(0x09, INPUT);
	pinMode(0x0a, INPUT);
	pinMode(0x0b, INPUT);
	pinMode(0x0c, INPUT);
	pinMode(0x0d, INPUT);
	pinMode(0x0e, INPUT);
	pinMode(0x0f, INPUT);
	Power3v3OutInit();
	PowerBatOutInit();
	PowerBatInInit();
}


void NCN_System_loop(void) {

	// hardware to software
	unsigned int gpioInputValue = 0;
	unsigned int adcValue[3] = {0xFFFF,0xFFFF,0xFFFF};
	int red = 0,green = 0,blue = 0,clear = 0;
	unsigned char sender_buffer[20] = {0x00};
	int i;
	int voltage = 0,current = 0;
	for ( i = 0; i < PINS_NUMBERS; i ++ ) {	//fill in the pin information
		if(GPIO_Pins[i] == 0x00) {			//GPIO input
			if(digitalRead(i) == HIGH) {
				gpioInputValue |= (1 << i);
			} else if(digitalRead(i) == LOW) {
				gpioInputValue &= ~(1 << i);
			}
		} else if(GPIO_Pins[i] == 0x01) {	//special function is used.
			if(i == 0 || i == 1 || i == 2) {	//pin0 or pin1 or pin2 is ADC input
				adcValue[i] = analogRead(i);
			} else if(i == 14 || i == 15) {		//pin 14 and pin 15 are I2C
				colorSensor.readRGB(&red,&green,&blue,&clear);
				voltage = ina219.readBusVoltageInt();
				current = ina219.readShuntCurrentInt();
			}
		}
	}
	sender_buffer[0] = (unsigned char)(gpioInputValue >> 8);
	sender_buffer[1] = (unsigned char)(gpioInputValue & 0xFF);
	sender_buffer[2] = (unsigned char)(adcValue[0] >> 8);
	sender_buffer[3] = (unsigned char)(adcValue[0] & 0xFF);
	sender_buffer[4] = (unsigned char)(adcValue[1] >> 8);
	sender_buffer[5] = (unsigned char)(adcValue[1] & 0xFF);
	sender_buffer[6] = (unsigned char)(adcValue[2] >> 8);
	sender_buffer[7] = (unsigned char)(adcValue[2] & 0xFF);
	sender_buffer[8] = (unsigned char)(red >> 8);
	sender_buffer[9] = (unsigned char)(red & 0xFF);
	sender_buffer[10] = (unsigned char)(green >> 8);
	sender_buffer[11] = (unsigned char)(green & 0xFF);
	sender_buffer[12] = (unsigned char)(blue >> 8);
	sender_buffer[13] = (unsigned char)(blue & 0xFF);
	sender_buffer[14] = (unsigned char)(clear >> 8);
	sender_buffer[15] = (unsigned char)(clear & 0xFF);
	sender_buffer[16] = (unsigned char)(voltage >> 8);
	sender_buffer[17] = (unsigned char)(voltage & 0xFF);
	sender_buffer[18] = (unsigned char)(current >> 8);
	sender_buffer[19] = (unsigned char)(current & 0xFF);

	NCN_System_upload(sender_buffer,20);
//
	__delay_cycles(20000000);
}

unsigned char i = 0;
#ifdef __cplusplus
extern "C" {
#endif
void NCN_System_download(unsigned char *data,unsigned char length) {
	if(length == 0x02) {
		if(data[0] < 16) {
			if((data[1] == 0x00 || data[1] == 0x01) && GPIO_Pins[data[0]] == 0x01) {
				digitalWrite(data[0],data[1]);
			} else if(data[1] == 0x02) {
				if(data[0] == 14 || data[0] == 15) {
					GPIO_Pins[14] =GPIO_Pins[15] = 0x00;
				}
				GPIO_Pins[data[0]] = 0x00;
				pinMode(data[0],INPUT);
			} else if(data[1] == 0x03) {
				if(data[0] == 14 || data[0] == 15) {
					GPIO_Pins[14] = 0x01;
					GPIO_Pins[15] = 0x01;
					soft_I2C.begin(0x39,14,15);
					colorSensor.Start();
					ina219.begin(INA219_ADDRESS);
					ina219.configure(INA219_RANGE_16V, INA219_GAIN_40MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
					ina219.calibrate(0.1, 0.5);
				} else if(data[0] >= 0x04){
					GPIO_Pins[data[0]] = 0x01;
					pinMode(data[0],1);
				} else {
					GPIO_Pins[data[0]] = 0x01;
				}
			}
		} else if (data[0] == 0x80) {
			if(data[1] == 0x01)
				Power3v3OutOpen();
			else if(data[1] == 0x00)
				Power3v3OutClose();
		} else if (data[0] == 0x81) {
			if(data[1] == 0x01)
				PowerBatOutOpen();
			else if(data[1] == 0x00)
				PowerBatOutClose();
		} else if (data[0] == 0x82) {
			if(data[1] == 0x01)
				PowerBatInOpen();
			else if(data[1] == 0x00)
				PowerBatInClose();
		}
	}
}
void NCN_System_Function_Switch(unsigned char *data,unsigned char length){
//	Function_Switch_flag = data[0];
}
#ifdef __cplusplus
}
#endif




