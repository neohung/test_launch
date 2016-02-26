/*
 * pin_map.c
 *
 *  Created on: 2015-8-19
 *      Author: Administrator
 */

#include "pin_map.h"
#include "msp430.h"
#define PORT1_OUT		((unsigned char *)0x21)
#define PORT1_IN		((unsigned char *)0x20)
#define PORT1_DIR		((unsigned char *)0x22)
#define PORT1_SEL		((unsigned char *)0x26)
#define PORT1_SEL2		((unsigned char *)0x41)
#define PORT1_REN		((unsigned char *)0x27)

#define PORT2_OUT		((unsigned char *)0x29)
#define PORT2_IN		((unsigned char *)0x28)
#define PORT2_DIR		((unsigned char *)0x2A)
#define PORT2_SEL		((unsigned char *)0x2E)
#define PORT2_SEL2		((unsigned char *)0x42)
#define PORT2_REN		((unsigned char *)0x2F)

#define PORT3_OUT		((unsigned char *)0x19)
#define PORT3_IN		((unsigned char *)0x18)
#define PORT3_DIR		((unsigned char *)0x1A)
#define PORT3_SEL		((unsigned char *)0x1B)
#define PORT3_SEL2		((unsigned char *)0x43)
#define PORT3_REN		((unsigned char *)0x10)

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
const PIN_node NCN_pin[16] = {
//		{.out = PORT1_OUT,	.in = PORT1_IN,	.dir = PORT1_DIR,	.sel = PORT1_SEL,	.sel2 = PORT1_SEL2,	.ren = PORT1_REN,	.bit = BIT0}
		{PORT1_IN,PORT1_OUT,PORT1_DIR,PORT1_SEL,PORT1_SEL2,PORT1_REN,BIT0},
		{PORT1_IN,PORT1_OUT,PORT1_DIR,PORT1_SEL,PORT1_SEL2,PORT1_REN,BIT6},
		{PORT1_IN,PORT1_OUT,PORT1_DIR,PORT1_SEL,PORT1_SEL2,PORT1_REN,BIT7},
		{PORT3_IN,PORT3_OUT,PORT3_DIR,PORT3_SEL,PORT3_SEL2,PORT3_REN,BIT4},	//empty
		{PORT3_IN,PORT3_OUT,PORT3_DIR,PORT3_SEL,PORT3_SEL2,PORT3_REN,BIT2},
		{PORT3_IN,PORT3_OUT,PORT3_DIR,PORT3_SEL,PORT3_SEL2,PORT3_REN,BIT3},
		{PORT3_IN,PORT3_OUT,PORT3_DIR,PORT3_SEL,PORT3_SEL2,PORT3_REN,BIT5},
		{PORT3_IN,PORT3_OUT,PORT3_DIR,PORT3_SEL,PORT3_SEL2,PORT3_REN,BIT6},
		{PORT2_IN,PORT2_OUT,PORT2_DIR,PORT2_SEL,PORT2_SEL2,PORT2_REN,BIT0},
		{PORT2_IN,PORT2_OUT,PORT2_DIR,PORT2_SEL,PORT2_SEL2,PORT2_REN,BIT1},
		{PORT2_IN,PORT2_OUT,PORT2_DIR,PORT2_SEL,PORT2_SEL2,PORT2_REN,BIT2},
		{PORT2_IN,PORT2_OUT,PORT2_DIR,PORT2_SEL,PORT2_SEL2,PORT2_REN,BIT3},
		{PORT2_IN,PORT2_OUT,PORT2_DIR,PORT2_SEL,PORT2_SEL2,PORT2_REN,BIT4},
		{PORT2_IN,PORT2_OUT,PORT2_DIR,PORT2_SEL,PORT2_SEL2,PORT2_REN,BIT5},
		{PORT2_IN,PORT2_OUT,PORT2_DIR,PORT2_SEL,PORT2_SEL2,PORT2_REN,BIT6},
		{PORT2_IN,PORT2_OUT,PORT2_DIR,PORT2_SEL,PORT2_SEL2,PORT2_REN,BIT7}
};
