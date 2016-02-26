/*
 * soft_i2c.cpp
 *
 *  Created on: 2015-8-19
 *      Author: Administrator
 */
#include "soft_i2c.h"
#include "msp430.h"
#include "pin_map.h"
#include "delay.h"
soft_i2c soft_I2C;
#define _scl_high_()	do{*NCN_pin[Pscl].out |= NCN_pin[Pscl].bit;}while(0)
#define _scl_low_()		do{*NCN_pin[Pscl].out &=~NCN_pin[Pscl].bit;}while(0)
#define _scl_output_()	do{*NCN_pin[Pscl].dir |= NCN_pin[Pscl].bit;}while(0)
#define _sda_input_()	do{*NCN_pin[Psda].dir &=~NCN_pin[Psda].bit;}while(0)
#define _sda_output_()	do{*NCN_pin[Psda].dir |= NCN_pin[Psda].bit;}while(0)
#define _sda_high_()	do{*NCN_pin[Psda].out |= NCN_pin[Psda].bit;}while(0)
#define _sda_low_()		do{*NCN_pin[Psda].out &=~NCN_pin[Psda].bit;}while(0)
#define _sda_value_()	(*NCN_pin[Psda].in & NCN_pin[Psda].bit ? 0x01 : 0x00)
inline void soft_i2c::i2c_start(void) {
	_sda_high_();
	_scl_high_();
	delay_us(5);
	_sda_low_();
	delay_us(5);
	_scl_low_();
}
inline void soft_i2c::i2c_stop(void) {
	_sda_low_();
	_scl_high_();
	delay_us(5);
	_sda_high_();
	delay_us(5);
}
inline unsigned char soft_i2c::i2c_checkACK(void) {
	unsigned char temp;
	_sda_input_();
	_scl_high_();
	delay_us(1);
	temp = _sda_value_();
	_scl_low_();
	_sda_output_();
	delay_us(1);
	return temp;
}
inline void soft_i2c::i2c_writeByte(unsigned char data) {
	unsigned char i = 0;
	_sda_output_();
	for(i = 0; i < 8; i++) {
		if( data & 0x80)
			_sda_high_();
		else
			_sda_low_();
		data <<= 1;
		_scl_high_();
		delay_us(1);
		_scl_low_();
		delay_us(1);
	}
	delay_us(1);
	i2c_checkACK();
}
inline unsigned char soft_i2c::i2c_readByte(unsigned char ack) {
	unsigned char i = 0,temp = 0,data = 0;
	_sda_input_();
	for(i = 0;i < 8;i++) {
		_scl_high_();
		delay_us(1);
		temp = _sda_value_();
		data = data << 1;
		if(temp)
			data = data | 0x01;
		_scl_low_();
		delay_us(1);
	}
	if(ack)
		i2c_NoACK();
	else
		i2c_ACK();
	return data;
}
inline void soft_i2c::i2c_ACK(void) {
	_sda_output_();
	delay_us(1);
	_sda_low_();
	delay_us(1);
	_scl_high_();
	delay_us(1);
	_scl_low_();
	delay_us(1);
}
inline void soft_i2c::i2c_NoACK(void) {
	_sda_output_();
	delay_us(1);
	_sda_high_();
	delay_us(1);
	_scl_high_();
	delay_us(1);
	_scl_low_();
	delay_us(1);
}
void soft_i2c::begin(void){
	this->Psda = 15;
	this->Pscl = 14;
	_sda_output_();
	_scl_output_();
}
void soft_i2c::begin(unsigned char address){
	this->address = address << 1;
	this->Psda = 15;
	this->Pscl = 14;
	_sda_output_();
	_scl_output_();
}
void soft_i2c::begin(unsigned char address,unsigned char Pscl,unsigned char Psda){
	this->address = address << 1;
	this->Psda = Psda;
	this->Pscl = Pscl;
	_sda_output_();
	_scl_output_();
}
void soft_i2c::i2c_write(unsigned char address,unsigned char reg,unsigned char data) {
	_sda_output_();
	i2c_start();
	i2c_writeByte((address << 1) );
	i2c_writeByte(reg);
	i2c_writeByte(data);
	i2c_stop();
}
unsigned char soft_i2c::i2c_read(unsigned char address,unsigned char reg) {
	unsigned char data = 0;
	_sda_output_();
	i2c_start();
	i2c_writeByte((address << 1) );
	i2c_writeByte(reg);
	i2c_start();
	i2c_writeByte((address << 1) +1);
	data = i2c_readByte(1);
	i2c_stop();
	return data;
}
void soft_i2c::i2c_read(unsigned char address,unsigned char reg,unsigned char *str,unsigned char length) {
	unsigned char i = 0;
	_sda_output_();
	i2c_start();
	i2c_writeByte((address << 1)  & 0xFE);
	i2c_writeByte(reg);
	i2c_stop();
	i2c_start();
	i2c_writeByte((address << 1)  | 0x01);
	for(i = 0;i < length - 1;i++) {
		str[i] = i2c_readByte(0);
	}
	str[i] = i2c_readByte(1);
	i2c_stop();
}
void soft_i2c::i2c_write(unsigned char address,unsigned char reg,unsigned char *str,unsigned char length) {
	unsigned char i = 0;
	_sda_output_();
	i2c_start();
	i2c_writeByte((address << 1) & 0xFE);
	i2c_writeByte(reg);
	for (i = 0; i < length;i++) {
		i2c_writeByte(str[i]);
	}
	i2c_stop();
}




void soft_i2c::i2c_write(unsigned char reg,unsigned char data) {
	_sda_output_();
	i2c_start();
	i2c_writeByte(this->address);
	i2c_writeByte(reg);
	i2c_writeByte(data);
	i2c_stop();
}
unsigned char soft_i2c::i2c_read(unsigned char reg) {
	unsigned char data = 0;
	_sda_output_();
	i2c_start();
	i2c_writeByte(this->address);
	i2c_writeByte(reg);
	i2c_start();
	i2c_writeByte(this->address+1);
	data = i2c_readByte(1);
	i2c_stop();
	return data;
}
void soft_i2c::i2c_read(unsigned char reg,unsigned char *str,unsigned char length) {
	unsigned char i = 0;
	_sda_output_();
	i2c_start();
	i2c_writeByte(this->address & 0xFE);
	i2c_writeByte(reg);
	i2c_stop();
	i2c_start();
	i2c_writeByte(this->address | 0x01);
	for(i = 0;i < length - 1;i++) {
		str[i] = i2c_readByte(0);
	}
	str[i] = i2c_readByte(1);
	i2c_stop();
}
void soft_i2c::i2c_write(unsigned char reg,unsigned char *str,unsigned char length) {
	unsigned char i = 0;
	_sda_output_();
	i2c_start();
	i2c_writeByte(this->address & 0xFE);
	i2c_writeByte(reg);
	for (i = 0; i < length;i++) {
		i2c_writeByte(str[i]);
	}
	i2c_stop();
}
