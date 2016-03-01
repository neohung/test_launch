#ifndef __I2C_H__
#define __I2C_H__


typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned int size_t;

#define I2C_READ 1 
#define I2C_WRITE 0 

#define BUFFER_LENGTH 16
//#define CPU_F 		((double) 16000000.0)//CPU Frequency
//#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
//#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

#define F_CPU 		(double)16000000L
#define TICKS_PER_MS (F_CPU / 1000) 
#define TICKS_PER_US (TICKS_PER_MS / 1000) 

const uint8_t DELAY_LONG = 32; 
const uint8_t DELAY_FULL = 8; 
const uint8_t DELAY_HALF = 4; 
const uint8_t DELAY_PART = 2; 


//BCSCTL1 = CALBC1_16MHZ;
//DCOCTL = CALDCO_16MHZ;


uint8_t rxBuffer[BUFFER_LENGTH]; 
uint8_t rxBufferIndex = 0; 
uint8_t rxBufferLength = 0; 
uint8_t txAddress = 0; 
uint8_t txBuffer[BUFFER_LENGTH]; 
uint8_t txBufferIndex = 0; 
uint8_t txBufferLength = 0; 
uint8_t transmitting = 0; 

typedef enum {
	INPUT = 0x00,
	OUTPUT = 0x01,
	HIGH = 0x01,
	LOW = 0x00
}PIN_MODE;



#endif

