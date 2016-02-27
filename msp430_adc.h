#ifndef __MSP430_ADC__
#define __MSP430_ADC__
#ifdef __cplusplus
	extern "C" {
#endif

#include "msp430g2553.h"
#include "stdbool.h"


void adc_port1_3_init(unsigned int* p);
void adc_start();
void adc_stop();

#ifdef __cplusplus
	}
#endif

#endif /* __MSP430_ADC__ */