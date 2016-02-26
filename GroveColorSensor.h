/*
 * GroveColorSensor.h
 *
 *  Created on: 2015-11-30
 *      Author: Administrator
 */

#ifndef GROVECOLORSENSOR_H_
#define GROVECOLORSENSOR_H_

#include "delay.h"
#define delay(x) delay_ms(x)

class GroveColorSensor
{
public:

	// Color Sensor LED Status
	int ledStatus;
	// Default constructor
	GroveColorSensor();
	// Constructor with parameters
	GroveColorSensor(
		  const unsigned char & triggerMode
		, const unsigned char & interruptSource
		, const unsigned char & interruptMode
		, const unsigned char & gainAndPrescaler
		, const unsigned char & sensorAddress);
	void Start();
	void readRGB();
	void readRGB(int *red, int *green, int *blue,int *clear);
	void resetRGB();
	void calculateCoordinate();
	void clearInterrupt();

private:

	// Set trigger mode. Including free mode, manually mode, single synchronization mode or so.
	void setTimingReg();
	// Set interrupt source
	void setInterruptSourceReg();
	// Set interrupt mode
	void setInterruptControlReg();
	// Set gain value and pre-scaler value
	void setGain();
	// Start ADC of the colour sensor
	void setEnableADC();

	// Used for storing the colour data
	int green_;
	int red_;
	int blue_;
	int clear_;

	unsigned char triggerMode_;
	unsigned char  interruptSource_;
	unsigned char  interruptMode_;
	unsigned char  gainAndPrescaler_;
	unsigned char  sensorAddress_;

};
#endif /* GROVECOLORSENSOR_H_ */
