
#include "msp430g2553.h"
#include "stdbool.h"

#include "msp430_uart.h"

void Measure_REF(unsigned int chan, unsigned int ref);
void Measure(unsigned int chan);

bool          ADCDone;             // ADC Done flag
unsigned int    ADCValue;                // Measured ADC Value
void uart_rx(char data);
void main(void)
{
 WDTCTL = WDTPW + WDTHOLD; // Stop WDT

 P1DIR |= BIT0 + BIT6; // Set the LEDs on P1.0, P1.6 as outputs
 //P1OUT = BIT0; // Set P1.0
 //P1OUT = BIT6;
 uart_init(uart_rx);
 //LPM0_bits
 //
  TA0CTL|=TACLR+TAIE;  //开启中断并清零
   TA0CCR0 = 32768;
   TA0CTL = TASSEL_2+ID_3+ MC_1;  // SMCLK, div 8, up mode,
   TACCTL0 = CCIE;        // Enable interrupts for CCR0.
 //
 
while(1)
{
	//UARTSendArray("ABC", 3);
	//__delay_cycles(500000);     
	if(ADCDone)            // If the ADC is done with a measurement
    {
		ADCDone = false;            // Clear flag
		UARTSendArray(ADCValue & 0x00FF,1);
		UARTSendArray((ADCValue >> 8)& 0x00FF,1);
	}
	//LPM0_bits+
	__bis_SR_register(GIE); // Enter LPM0, interrupts enabled
}
}

void uart_rx(char data)
{
	UARTSendArray(&data, 1);
		  switch (data){
		  case 'A':
			Measure_REF(INCH_10,0);
		  break;
		  default:
		  break;
	  }

}

__attribute__((interrupt(TIMER0_A0_VECTOR)))
void neo_TimerA_procedure(void)
{ 
P1OUT ^= BIT6;
//TA0CCR0 += 32768;    // Add offset to CCR0
//__bic_SR_register_on_exit(CPUOFF);
// Enable CPU so the main while loop continues

}

/***********************************************************************************
* ADC interrupt routine. Pulls CPU out of sleep mode for the main loop.
************************************************************************************/
__attribute__((interrupt(ADC10_VECTOR)))
void ADC10_ISR (void)
{
    ADCValue = ADC10MEM;                    // Saves measured value.
    ADCDone = true;                         // Sets flag for main loop.
    //__bic_SR_register_on_exit(CPUOFF);// Enable CPU so the main while loop continues
}


/****************************************************************************************
* Reads ADC 'chan' once using an internal reference, 'ref' determines if the
*   1.5V reference is used.
**************************************************************************************/
void Measure_REF(unsigned int chan, unsigned int ref)
{
    ADC10CTL0 &= ~ENC;                     // Disable ADC
    ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ref + ADC10IE;
                        // Use reference,
                           // 16 clock ticks, internal reference on
                     // ADC On, enable ADC interrupt, Internal = 'ref'
    ADC10CTL1 = ADC10SSEL_3 + chan;     // Set 'chan', SMCLK
    __delay_cycles (128);               // Delay to allow Ref to settle
    ADC10CTL0 |= ENC + ADC10SC;         // Enable and start conversion
}

/************************************************************************************
* Reads ADC 'chan' once using AVCC as the reference.
****************************************************************************************/
void Measure(unsigned int chan)
{
    ADC10CTL0 &= ~ENC;                       // Disable ADC
    ADC10CTL0 = ADC10SHT_3 + ADC10ON + ADC10IE;
                                        // 16 clock ticks, ADC On, enable ADC interrupt
    ADC10CTL1 = ADC10SSEL_3 + chan;        // Set 'chan', SMCLK
    ADC10CTL0 |= ENC + ADC10SC;         // Enable and start conversion
}
