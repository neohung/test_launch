#ifndef __MSP430_PRINTF__
#define __MSP430_PRINTF__
#ifdef __cplusplus
	extern "C" {
#endif

#include "msp430g2553.h"
#include "stdbool.h"
#include "stdarg.h"


typedef struct
{
 short level ;
 short token ;
 short bsize ;
 char fd ;
 unsigned flags ;
 unsigned char hold ;
 unsigned char * buffer ;
 unsigned char * curp ;
 unsigned istemp ;
}FILE ;


int printf(const char * format, ...);

#ifdef __cplusplus
	}
#endif

#endif /* __MSP430_PRINTF__ */