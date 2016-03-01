
#include "i2c.h"



static uint8_t _pinSDA;
static uint8_t _pinSCL;
void delayI2Cms(uint16_t delay) 
{ 
     while (delay--) __delay_cycles(TICKS_PER_MS); 
} 
void delayI2Cus(uint16_t delay) 
{ 
     while (delay--) __delay_cycles(TICKS_PER_US); 
} 

const unsigned char pin_list [] = {
		BIT0,BIT6,BIT7,								//P1
		BIT4,BIT2,BIT3,BIT5,BIT6,					//P3
		BIT0,BIT1,BIT2,BIT3,BIT4,BIT5,BIT6,BIT7,	//P2

};
/*
*******   Pin list    *******
** pin ** Port.pin ****** Mode **************
 * 00.		P1.0		ADC							31
 * 01.		P1.6		ADC							21
 * 02.		P1.7		ADC							22
 * 03.		P3.4									14
 * 04.		P3.2		PWM							12		TA1.1
 * 05.		P3.3		PWM							13		TA1.2
 * 06.		P3.5		PWM							18		TA0.1
 * 07.		P3.6		PWM							19		TA0.2
 * 08.		P2.0		UART_TXO					9		TA1.0
 * 09.		P2.1		UART_RDI					10		TA1.1
 * 10.		P2.2		SPI MISO					11
 * 11.		P2.3		SPI_MOSI					15
 * 12.		P2.4		SPI_CLK						16
 * 13.		P2.5		SPI_CS						17
 * 14.		P2.6		I2C_SCL						26
 * 15.		P2.7		I2C_SDA						25
 ********************************************
 */
void pinMode(unsigned char pin,unsigned char mode) {
	if(pin >= 16)
		return;
	else if(pin > 7) {				//No.08~15 is P2
		P2SEL &=~pin_list[pin];
		P2SEL2 &=~pin_list[pin];
		mode == OUTPUT ? (P2DIR |= pin_list[pin]) : (P2DIR &=~pin_list[pin]);
	} else if(pin >= 4) {			//No.04~7 is P3
		P3SEL &=~ pin_list[pin];
		mode == OUTPUT ? (P3DIR |= pin_list[pin]) : (P3DIR &=~pin_list[pin]);
	}
	else if(pin > 2) {		//No.03
		mode == OUTPUT ? (P3DIR |= pin_list[pin]) : (P3DIR &=~pin_list[pin]);
	} else {				//No.00~02
		if(mode == INPUT) {
			//pin_adc[pin] = 0x01;
			P1DIR &=~pin_list[pin];
		} else {
			//pin_adc[pin] = 0x00;
			P1DIR |= pin_list[pin];
		}
	}
}

void digitalWrite(unsigned char pin,unsigned char value) {
	if(pin >= 16)
		return;
	else if(pin > 7) {		//No.08~15 is P2
		value == HIGH ? (P2OUT |= pin_list[pin]) : (P2OUT &=~pin_list[pin]);
	} else if(pin >= 4) {	//No.04~7 is P3
		P3SEL &=~ pin_list[pin];
		value == HIGH ? (P3OUT |= pin_list[pin]) : (P3OUT &=~pin_list[pin]);
	}
	else if(pin > 2) {	//No.03
		value == HIGH ? (P3OUT |= pin_list[pin]) : (P3OUT &=~pin_list[pin]);
	} else {			//No.00~02
		ADC10AE0 &= ~pin_list[pin];
		value == HIGH ? (P1OUT |= pin_list[pin]) : (P1OUT &=~pin_list[pin]);
	}
}

unsigned char digitalRead(unsigned char pin) {
	if(pin >= 16)
		return 0xFF;
	else if(pin > 7) {  //No.08~15 is P2
		if(P2IN & pin_list[pin])
			return HIGH;
		else
			return LOW;
	} else if (pin >= 4) {		//No.04~7 is P3
		P3SEL &=~ pin_list[pin];
		if(P3IN & pin_list[pin])
			return HIGH;
		else
			return LOW;
	}
	else if(pin > 2) {		//No.03
		if(P3IN & pin_list[pin])
			return HIGH;
		else
			return LOW;
	} else {				//No.00~02
		ADC10AE0 &= ~pin_list[pin];
		if(P1IN & pin_list[pin])
			return HIGH;
		else
			return LOW;
	}
}


void SoftwareWire(uint8_t pinSDA, uint8_t pinSCL) { 
     _pinSDA = pinSDA; 
     _pinSCL = pinSCL; 
} 

void SoftwareWire_begin() { 
     pinMode(_pinSDA, OUTPUT); 
     digitalWrite(_pinSDA, HIGH); 
     pinMode(_pinSCL, OUTPUT); 
     digitalWrite(_pinSCL, HIGH); 
     rxBufferIndex = 0; 
     rxBufferLength = 0; 
     txBufferIndex = 0; 
     txBufferLength = 0; 
} 


uint8_t SoftwareWire_writeI2C(uint8_t data) { 
     pinMode(_pinSDA, OUTPUT); 
	 uint8_t i;
     for (i=0; i < 8; ++i ){    
		 //先拉Hi/Lo Data Pin
         if (data & 0x80) digitalWrite(_pinSDA, HIGH); 
         else digitalWrite(_pinSDA, LOW); 
		 //接著拉clock Pin
         digitalWrite(_pinSCL, HIGH); 
		 //左移8次
         data <<= 1;
		 //Clock Hi 4us
         delayI2Cus(DELAY_HALF); 
         digitalWrite(_pinSCL, LOW); 
		 //Clock Lo 4us
         delayI2Cus(DELAY_HALF);
		 //一個周期約8us ~= 125KHz		 
     } 
     pinMode(_pinSDA, INPUT); 
     digitalWrite(_pinSCL, HIGH); 
	 //確認對方是否ACK
     uint8_t result = !digitalRead(_pinSDA); 
     if (result) digitalWrite(_pinSDA, LOW); 
     delayI2Cus(DELAY_HALF); 
     digitalWrite(_pinSCL, LOW); 
     delayI2Cus(DELAY_FULL); 
     return result; 
 } 
 
size_t SoftwareWire_write_with_transmitting(uint8_t data) 
{ 
     if (transmitting){ 
         if (txBufferLength >= BUFFER_LENGTH) { 
             return 0; 
        } 
         txBuffer[txBufferIndex] = data; 
        ++txBufferIndex; 
         txBufferLength = txBufferIndex; 
    } else { 
         SoftwareWire_writeI2C(data); 
     } 
     return 1; 
 } 

size_t SoftwareWire_write_Data(const uint8_t *data, size_t length) 
{ 
    if (transmitting) { 
	    //transmitting=1表示transmitting
		size_t i;
        for (i = 0; i < length; ++i) SoftwareWire_write_with_transmitting(data[i]); 
    } else { 
	 //transmitting=0表示直接寫入
	 size_t i;
    	for (i = 0; i < length; ++i) SoftwareWire_writeI2C(data[i]); 
    } 
    return length; 
} 


uint8_t SoftwareWire_readI2C(uint8_t last) { 
    uint8_t data = 0;
	//Data Pin先拉Lo	
    digitalWrite(_pinSDA, LOW); 
    delayI2Cus(DELAY_LONG);
    //設Data Pin為Input	
    pinMode(_pinSDA, INPUT);
    //等4us	
     delayI2Cus(DELAY_HALF); 
	 uint8_t i;
     for (i= 0; i < 8; i++) { 
         data <<= 1; 
         digitalWrite(_pinSCL, HIGH); 
		 //Clock拉Hi後2us讀資料
         delayI2Cus(DELAY_PART); 
         if (digitalRead(_pinSDA)) data |= 1; 
         //再經過2us後Clock拉Lo
		 delayI2Cus(DELAY_PART); 
         digitalWrite(_pinSCL, LOW);
		 //Clock拉Lo維持4us
         delayI2Cus(DELAY_HALF); 
		 //Clock拉Hi共4us,拉Lo共4us
     } 
     pinMode(_pinSDA, OUTPUT); 
	 //拉Lo表示Ack
     digitalWrite(_pinSDA, last); 
     digitalWrite(_pinSCL, HIGH); 
     delayI2Cus(DELAY_HALF); 
	 //歸0
     digitalWrite(_pinSCL, LOW); 
     digitalWrite(_pinSDA, LOW); 
     delayI2Cus(DELAY_LONG); 
     return data; 
 } 
 
//return uint8_t?
void SoftwareWire_readI2C_Data(uint8_t* data, uint8_t length) 
{ 
     if (length > 1) { 
         while (--length) *data++= SoftwareWire_readI2C(false);  //拉Lo表示Ack
     } 
     *data = SoftwareWire_readI2C(true); //拉Hi表示NAck即結束
} 


int SoftwareWire_read(void) 
{ 
     int value = -1; 
     if(rxBufferIndex < rxBufferLength){ 
         value = rxBuffer[rxBufferIndex]; 
         ++rxBufferIndex; 
     } 
     return value; 
} 

 
uint8_t SoftwareWire_startI2C(uint8_t address, uint8_t RW) { 
     pinMode(_pinSDA, OUTPUT); 
	 //Clock Hi的過程中Data Pin拉Lo , 表示Start Condition
     digitalWrite(_pinSDA, LOW); 
	 //Clock拉Lo, Clock下次拉Hi時表示開始送訊號
     digitalWrite(_pinSCL, LOW); 
     delayI2Cus(DELAY_FULL); 
     return SoftwareWire_writeI2C((address << 1) + RW); 
 } 

bool SoftwareWire_restartI2C(uint8_t address, uint8_t RW) { 
     digitalWrite(_pinSDA, HIGH); 
     digitalWrite(_pinSCL, HIGH); 
     delayI2Cus(DELAY_FULL); 
     return SoftwareWire_startI2C(address, RW); 
} 

 void SoftwareWire_stopI2C(void) { 
     pinMode(_pinSDA, OUTPUT); 
     digitalWrite(_pinSDA, LOW); 
     delayI2Cus(DELAY_FULL); 
	 //Clock Hi的過程中Data Pin拉Hi , 表示Stop Condition
     digitalWrite(_pinSCL, HIGH); 
     delayI2Cus(DELAY_FULL); 
     digitalWrite(_pinSDA, HIGH); 
     delayI2Cus(DELAY_FULL); 
} 


 void SoftwareWire_beginTransmission(uint8_t address) 
{ 
     transmitting = 1; 
     txAddress = address; 
     txBufferIndex = 0; 
     txBufferLength = 0; 
} 

uint8_t SoftwareWire_writeI2C_data(uint8_t* data, size_t length) 
 { 
     uint8_t result = 0;
     uint8_t i;	 
     for (i=0; i < length; i++) result = SoftwareWire_writeI2C(data[i]); 
     return result; 
 } 


uint8_t SoftwareWire_endTransmission(void) 
{ 
     uint8_t result = 0; 
     SoftwareWire_startI2C(txAddress, I2C_WRITE); 
	 result += SoftwareWire_writeI2C_data(txBuffer, txBufferLength); 
     SoftwareWire_stopI2C(); 
     txBufferIndex = 0; 
     txBufferLength = 0; 
     transmitting = 0; 
     return result; 
} 
