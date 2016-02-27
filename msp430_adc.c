#include "msp430_adc.h"

static bool          ADCDone;             // ADC Done flag
static unsigned int  ADCValue = 0;                // Measured ADC Value
static unsigned int*  pADCValue;
void adc_port1_3_init(unsigned int* p)
{
 P1SEL |= BIT3; //set p1.3 as ADC  
 ADC10CTL1 = INCH_3 + ADC10DIV_3 ; // Channel 3, ADC10CLK/3
 ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
 ADC10AE0 |= BIT3;               // ADC input enable P1.3
 pADCValue = p;
}

void adc_start()
{
	ADC10CTL0 |= ENC + ADC10SC;	
}

void adc_stop()
{
	ADC10CTL0 &= ~ENC;	
}

/***********************************************************************************
* ADC interrupt routine. Pulls CPU out of sleep mode for the main loop.
************************************************************************************/
__attribute__((interrupt(ADC10_VECTOR)))
void ADC10_ISR (void)
{
	ADCValue = ADC10MEM & 0x3FF;                    // Saves measured value.
	*pADCValue = ADCValue;
    ADCDone = true;                         // Sets flag for main loop.
    __bic_SR_register_on_exit(CPUOFF);// Enable CPU so the main while loop continues
}
