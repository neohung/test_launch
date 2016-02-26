/*
 * delay.h
 *
 *  Created on: 2015-8-20
 *      Author: Administrator
 */

#ifndef DELAY_H_
#define DELAY_H_

#define CPU_F 		((double) 16000000.0)//CPU Frequency
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

#endif /* DELAY_H_ */
