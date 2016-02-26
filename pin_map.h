/*
 * pin_map.h
 *
 *  Created on: 2015-8-19
 *      Author: Administrator
 *      versio: v0_0_1
 */

#ifndef PIN_MAP_H_
#define PIN_MAP_H_
typedef struct {
	volatile unsigned char *in;
	volatile unsigned char *out;
	volatile unsigned char *dir;
	volatile unsigned char *sel;
	volatile unsigned char *sel2;
	volatile unsigned char *ren;
	unsigned char bit;
}PIN_node;
extern const PIN_node NCN_pin[16];

#endif /* PIN_MAP_H_ */
