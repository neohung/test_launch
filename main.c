
#include "msp430g2553.h"
#include "stdbool.h"

#include "msp430_uart.h"
#include "msp430_adc.h"
#include "msp430_printf.h"
#include "INA219.h"
#include "i2c.h"

bool          ADCDone;             // ADC Done flag
unsigned int  ADCValue = 0;                // Measured ADC Value

void uart_rx(char data);

#define TXD BIT1
#define RXD BIT2
#define FCPU 			1600000
#define BAUDRATE 		9600
#define BIT_TIME        (FCPU / BAUDRATE)
#define HALF_BIT_TIME   (BIT_TIME / 2)
static volatile unsigned char bitCount;
static volatile unsigned int TXByte;
static volatile unsigned int RXByte;
 void uart_init_test(void)
{
     P1SEL |= TXD;
     P1DIR |= TXD;

    // P1IES |= RXD; 		// RXD Hi/lo edge interrupt
    // P1IFG &= ~RXD; 		// Clear RXD (flag) before enabling interrupt
     P1IE  |= RXD; 		// Enable RXD interrupt
}

void uart_putc_test(unsigned char c)
{
     TXByte = c;

     //while(isReceiving); 					// Wait for RX completion

     CCTL0 = OUT; 							// TXD Idle as Mark
     TACTL = TASSEL_2 + MC_2; 				// SMCLK, continuous mode

     bitCount = 0xA; 						// Load Bit counter, 8 bits + ST/SP
     CCR0 = TAR; 							// Initialize compare register

     CCR0 += BIT_TIME; 						// Set time till first bit
     TXByte |= 0x100; 						// Add stop bit to TXByte (which is logical 1)
     TXByte = TXByte << 1; 					// Add start bit (which is logical 0)

     CCTL0 = CCIS_0 + OUTMOD_0 + CCIE + OUT; // Set signal, intial value, enable interrupts

    // while ( CCTL0 & CCIE ); 				// Wait for previous TX completion
}


int main(void)
{
 WDTCTL = WDTPW + WDTHOLD; // Stop WDT
 BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1Mhz
 DCOCTL = CALDCO_1MHZ;

 P1DIR |= BIT0; // Set the LEDs on P1.0, P1.6 as outputs
 P1OUT = 0; // Set P1.0
 //P1OUT = 0;
uart_init(uart_rx);
 //  __delay_cycles(100000);	
printf("%s\r","Neo Test now");
SoftwareWire(11,12);
printf("%s\r","SoftwareWire 11 12");
SoftwareWire_begin();
printf("%s\r","SoftwareWire begin");
uint8_t addr=0x41;
/*
SoftwareWire_beginTransmission(addr);
SoftwareWire_write_with_transmitting((0x00));
SoftwareWire_write_with_transmitting((0x01));
SoftwareWire_write_with_transmitting((0x8f));
SoftwareWire_endTransmission();
printf("%s\r","SoftwareWire finish");
*/
//SoftwareWire_requestFrom(addr,2);
//printf("Data: 0x%x\r",SoftwareWire_read());
//printf("Data: 0x%x\r",SoftwareWire_read());
 printf("Start r=%d\r" ,SoftwareWire_startI2C(0x40,1));
 //
//          |             |   |
//          |   TMP100   10k 10k     MSP430G2xx3
//          |   -------   |   |   -------------------
//          +--|Vcc SDA|<-|---+->|P1.7/UCB0SDA    XIN|-
//          |  |       |  |      |                   |
//          +--|A1,A0  |  |      |               XOUT|-
//             |       |  |      |                   |
//          +--|Vss SCL|<-+------|P1.6/UCB0SCL   P1.0|---> LED
//         \|/  -------          |                   |
//

  __bis_SR_register(GIE);  
  __delay_cycles(100000);	
//P1OUT = BIT0;
  while(1){
	   __bis_SR_register(CPUOFF + GIE);  
  }

return 0;
}


/*
//For I2C_TX
 __attribute__((interrupt(USCIAB0TX_VECTOR)))
void USCIAB0TX_ISR(void)
{
}
*/

void uart_rx(char data)
{
	//UARTSendArray(&data, 1);
		  switch (data){
		  case 'A':
			//Measure_REF(INCH_10,0);
		  break;
		  default:
		  break;
	  }

}

__attribute__((interrupt(TIMER0_A0_VECTOR)))
void neo_TimerA_procedure(void)
{ 

//P1OUT ^= BIT0;
//TA0CCR0 += 32768;    // Add offset to CCR0
//__bic_SR_register_on_exit(CPUOFF);
// Enable CPU so the main while loop continues
}
