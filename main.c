
#include "msp430g2553.h"
#include "stdbool.h"

#include "msp430_uart.h"
#include "msp430_adc.h"
#include "msp430_printf.h"
#include "INA219.h"

bool          ADCDone;             // ADC Done flag
unsigned int  ADCValue = 0;                // Measured ADC Value

void uart_rx(char data);
 
int main(void)
{
 WDTCTL = WDTPW + WDTHOLD; // Stop WDT
 BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1Mhz
 DCOCTL = CALDCO_1MHZ;

 P1DIR |= BIT0; // Set the LEDs on P1.0, P1.6 as outputs
 //P1OUT = BIT0; // Set P1.0
 P1OUT = 0;
 uart_init(uart_rx);
   __delay_cycles(100000);	
   printf("%s\r","Neo Test now");
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
