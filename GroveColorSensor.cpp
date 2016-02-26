/*
 * GroveColorSensor.cpp
 *
 *  Created on: 2015-11-30
 *      Author: Administrator
 */
#include "GroveColorSensor.h"
#include "Registers.h"
#include "soft_i2c.h"

GroveColorSensor::GroveColorSensor()
	: triggerMode_(INTEG_MODE_FREE | INTEG_PARAM_PULSE_COUNT1)
	, interruptSource_(INT_SOURCE_CLEAR)
	, interruptMode_(INTR_LEVEL | INTR_PERSIST_EVERY)
	, gainAndPrescaler_(GAIN_1 | PRESCALER_1)
	, sensorAddress_(COLOR_SENSOR_ADDR)
{

}

// Constructor with parameters
GroveColorSensor::GroveColorSensor(
	  const unsigned char & triggerMode
	, const unsigned char & interruptSource
	, const unsigned char & interruptMode
	, const unsigned char & gainAndPrescaler
	, const unsigned char & sensorAddress)
	: triggerMode_(triggerMode)
	, interruptSource_(interruptSource)
	, interruptMode_(interruptMode)
	, gainAndPrescaler_(gainAndPrescaler)
	, sensorAddress_(sensorAddress)
{}
void GroveColorSensor::Start() {
	GroveColorSensor::setTimingReg();
	GroveColorSensor::setInterruptSourceReg();
	GroveColorSensor::setInterruptControlReg();
	GroveColorSensor::setGain();
	GroveColorSensor::setEnableADC();
}
void GroveColorSensor::setTimingReg()
{
	soft_I2C.i2c_write(sensorAddress_,REG_TIMING,triggerMode_);
	delay(10);
}

void GroveColorSensor::setInterruptSourceReg()
{
	soft_I2C.i2c_write(sensorAddress_,REG_INT_SOURCE,interruptSource_);
	delay(10);
}

void GroveColorSensor::setInterruptControlReg()
{
	soft_I2C.i2c_write(sensorAddress_,REG_INT,interruptMode_);
	delay(10);
}

void GroveColorSensor::setGain()
{
	soft_I2C.i2c_write(sensorAddress_,REG_GAIN,gainAndPrescaler_);
}

void GroveColorSensor::setEnableADC()
{
	soft_I2C.i2c_write(sensorAddress_,REG_CTL,CTL_DAT_INIITIATE);
	delay(10);
}

void GroveColorSensor::clearInterrupt()
{
	soft_I2C.i2c_read(sensorAddress_,CLR_INT);

//	Wire.beginTransmission(sensorAddress_);
//	Wire.write(CLR_INT);
//	Wire.endTransmission();
}

void GroveColorSensor::readRGB()
{
	unsigned char temp[8] = {0};
	soft_I2C.i2c_read(sensorAddress_,REG_BLOCK_READ,temp,8);
	delay(100);
	green_	= temp[1];
	green_  = (green_ << 8) + temp[0];
	red_	= temp[3];
	red_  	= (red_ << 8) + temp[2];
	blue_	= temp[5];
	blue_  	= (blue_ << 8) + temp[4];
	clear_	= temp[7];
	clear_  = (clear_ << 8) + temp[6];

}
void GroveColorSensor::resetRGB(){
	green_ = red_ = blue_ = clear_ = 0;
}
void GroveColorSensor::readRGB(int *red, int *green, int *blue,int *clear)
{
	unsigned char temp[8] = {0};
	soft_I2C.i2c_read(sensorAddress_,REG_BLOCK_READ,temp,8);
	delay(100);
	*green	= temp[1];
	*green  = ((*green) << 8) + temp[0];
	*red	= temp[3];
	*red  	= ((*red) << 8) + temp[2];
	*blue	= temp[5];
	*blue  	= ((*blue) << 8) + temp[4];
	*clear	= temp[7];
	*clear  = ((*clear) << 8) + temp[6];


//	Wire.beginTransmission(sensorAddress_);
//	Wire.write(REG_BLOCK_READ);
//	Wire.endTransmission();
//
//	Wire.beginTransmission(sensorAddress_);
//	Wire.requestFrom(sensorAddress_, 8);
//	delay(100);
//
//	// if two bytes were received
//	if(8 <= Wire.available())
//	{
//		int i;
//		for(i = 0; i < 8; ++i)
//		{
//			readingdata_[i] = Wire.read();
//			//Serial.println(readingdata_[i], BIN);
//		}
//	}
//	green_	= readingdata_[1] * 256 + readingdata_[0];
//	red_ 	= readingdata_[3] * 256 + readingdata_[2];
//	blue_	= readingdata_[5] * 256 + readingdata_[4];
//	clear_	= readingdata_[7] * 256 + readingdata_[6];
//
//	double tmp;
//	int maxColor;
//
//	if ( ledStatus == 1 )
//	{
//		red_  = red_  * 1.70;
//		blue_ = blue_ * 1.35;
//
//		maxColor = max(red_, green_);
//		maxColor = max(maxColor, blue_);
//
//		if(maxColor > 255)
//		{
//			tmp = 250.0/maxColor;
//			green_	*= tmp;
//			red_ 	*= tmp;
//			blue_	*= tmp;
//		}
//	}
//	if ( ledStatus == 0 )
//	{
//		maxColor = max(red_, green_);
//		maxColor = max(maxColor, blue_);
//
//		tmp = 250.0/maxColor;
//		green_	*= tmp;
//		red_ 	*= tmp;
//		blue_	*= tmp;
//
//	}
//
//	int minColor = min(red_, green_);
//	minColor = min(maxColor, blue_);
//	maxColor = max(red_, green_);
//	maxColor = max(maxColor, blue_);
//
//	int greenTmp = green_;
//	int redTmp 	 = red_;
//	int blueTmp	 = blue_;
//
////when turn on LED, need to adjust the RGB data,otherwise it is almost the white color
//	if(red_ < 0.8*maxColor && red_ >= 0.6*maxColor)
//	{
//		red_ *= 0.4;
//    }
//	else if(red_ < 0.6*maxColor)
//	{
//		red_ *= 0.2;
//    }
//
//	if(green_ < 0.8*maxColor && green_ >= 0.6*maxColor)
//	{
//		green_ *= 0.4;
//    }
//	else if(green_ < 0.6*maxColor)
//	{
//		if (maxColor == redTmp && greenTmp >= 2*blueTmp && greenTmp >= 0.2*redTmp)				//orange
//		{
//			green_ *= 5;
//		}
//		green_ *= 0.2;
//    }
//
//	if(blue_ < 0.8*maxColor && blue_ >= 0.6*maxColor)
//	{
//		blue_ *= 0.4;
//    }
//	else if(blue_ < 0.6*maxColor)
//	{
//		if (maxColor == redTmp && greenTmp >= 2*blueTmp && greenTmp >= 0.2*redTmp)				//orange
//		{
//			blue_ *= 0.5;
//		}
//		if (maxColor == redTmp && greenTmp <= blueTmp && blueTmp >= 0.2*redTmp)					//pink
//		{
//			blue_  *= 5;
//		}
//		blue_ *= 0.2;
//    }
//
//	minColor = min(red_, green_);
//	minColor = min(maxColor, blue_);
//	if(maxColor == green_ && red_ >= 0.85*maxColor && minColor == blue_)						//yellow
//	{
//		red_ = maxColor;
//		blue_ *= 0.4;
//    }
//
//	*red   = red_;
//	*green = green_;
//	*blue  = blue_;
}

void GroveColorSensor::calculateCoordinate()
{
//	double X;
//	double Y;
//	double Z;
//	double x;
//	double y;
//
//	X = (-0.14282) * red_ + (1.54924) * green_ + (-0.95641) * blue_;
//	Y = (-0.32466) * red_ + (1.57837) * green_ + (-0.73191) * blue_;
//	Z = (-0.68202) * red_ + (0.77073) * green_ + (0.563320) * blue_;
//
//	x = X / (X + Y + Z);
//	y = Y / (X + Y + Z);
//
//	if( (X > 0) && ( Y > 0) && ( Z > 0) )
//	{
//		Serial.println("The x,y values are(");
//		Serial.print(x, 2);
//		Serial.print(" , ");
//		Serial.print(y, 2);
//		Serial.println(")");
//	}
//	else
//		Serial.println("Error: overflow!");
}




