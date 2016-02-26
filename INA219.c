/*
 * INA219.cpp
 *
 *  Created on: 2015-12-4
 *      Author: Administrator
 */
/*
INA219.cpp - Class file for the INA219 Zero-Drift, Bi-directional Current/Power Monitor Arduino Library.

Version: 1.0.0
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "INA219.h"
#include "soft_i2c.h"
#include "math.h"

int INA219_readRegister16(unsigned char reg)
{
    int value;
    unsigned char temp[2] = {0};
	soft_i2c_read_data(inaAddress,reg,temp,2);
	value = temp[0];
	value = (value << 8) | temp[1];
    return value;
}

void INA219_writeRegister16(unsigned char reg, unsigned int val)
{
    unsigned char temp[2] = {(unsigned char)(val >> 8),(unsigned char)(val & 0xFF)};
    soft_i2c_write_data(inaAddress,reg,temp,2);
}

unsigned char INA219_begin(unsigned char address)
{
	//soft_I2C.begin(address,12,13);
    inaAddress = address;
    return 1;
}

ina219_mode_t INA219_getMode(void)
{
    unsigned int value;

    value = INA219_readRegister16(INA219_REG_CONFIG);
    value &= 0x0007;
    return (ina219_mode_t)value;
}

/*
unsigned char INA219::configure(ina219_range_t range, ina219_gain_t gain, ina219_busRes_t busRes, ina219_shuntRes_t shuntRes, ina219_mode_t mode)
{
	unsigned int config = 0;

    config |= (range << 13 | gain << 11 | busRes << 7 | shuntRes << 3 | mode);

    switch(range)
    {
        case INA219_RANGE_32V:
            vBusMax = 32.0f;
            break;
        case INA219_RANGE_16V:
            vBusMax = 16.0f;
            break;
    }

    switch(gain)
    {
        case INA219_GAIN_320MV:
            vShuntMax = 0.32f;
            break;
        case INA219_GAIN_160MV:
            vShuntMax = 0.16f;
            break;
        case INA219_GAIN_80MV:
            vShuntMax = 0.08f;
            break;
        case INA219_GAIN_40MV:
            vShuntMax = 0.04f;
            break;
    }

    writeRegister16(INA219_REG_CONFIG, config);

    return 1;
}

unsigned char INA219::calibrate(float rShuntValue, float iMaxExpected)
{
	unsigned int calibrationValue = 4096;
    rShunt = rShuntValue;
    powerLSB = 0.002;
    currentLSB = 0.0001;
//    float minimumLSB;


//    minimumLSB = iMaxExpected / 32767;
//
//    currentLSB = (unsigned int)(minimumLSB * 100000000);
//    currentLSB /= 100000000;
//    currentLSB /= 0.0001;
////    currentLSB = ceil(currentLSB);
//    currentLSB *= 0.0001;
//
//    powerLSB = currentLSB * 20;
//
//    calibrationValue = (unsigned int)((0.04096) / (currentLSB * rShunt));
//
    writeRegister16(INA219_REG_CALIBRATION, calibrationValue);

    return 1;
}

float INA219::getMaxPossibleCurrent(void)
{
    return (vShuntMax / rShunt);
}

float INA219::getMaxCurrent(void)
{
    float maxCurrent = (currentLSB * 32767);
    float maxPossible = getMaxPossibleCurrent();

    if (maxCurrent > maxPossible)
    {
        return maxPossible;
    } else
    {
        return maxCurrent;
    }
}

float INA219::getMaxShuntVoltage(void)
{
    float maxVoltage = getMaxCurrent() * rShunt;

    if (maxVoltage >= vShuntMax)
    {
        return vShuntMax;
    } else
    {
        return maxVoltage;
    }
}

float INA219::getMaxPower(void)
{
    return (getMaxCurrent() * vBusMax);
}

float INA219::readBusPower(void)
{
    return (readRegister16(INA219_REG_POWER) * powerLSB);
}

float INA219::readShuntCurrent(void)
{
    return (readRegister16(INA219_REG_CURRENT) * currentLSB);
}

float INA219::readShuntVoltage(void)
{
    float voltage;

    voltage = readRegister16(INA219_REG_SHUNTVOLTAGE);

    return (voltage / 100000);
}

float INA219::readBusVoltage(void)
{
	unsigned int voltage;

    voltage = readRegister16(INA219_REG_BUSVOLTAGE);
    voltage >>= 3;

    return (voltage * 0.004);
}
int INA219::readShuntCurrentInt(void)
{
    return readRegister16(INA219_REG_CURRENT);
}
int INA219::readBusVoltageInt(void)
{
	unsigned int voltage;

    voltage = readRegister16(INA219_REG_BUSVOLTAGE);
    voltage >>= 3;

    return voltage;
}
ina219_range_t INA219::getRange(void)
{
	unsigned int value;

    value = readRegister16(INA219_REG_CONFIG);
    value &= 0x2000;
    value >>= 13;

    return (ina219_range_t)value;
}

ina219_gain_t INA219::getGain(void)
{
	unsigned int value;

    value = readRegister16(INA219_REG_CONFIG);
    value &= 0x1800;
    value >>= 11;

    return (ina219_gain_t)value;
}

ina219_busRes_t INA219::getBusRes(void)
{
	unsigned int value;

    value = readRegister16(INA219_REG_CONFIG);
    value &= 0x0780;
    value >>= 7;

    return (ina219_busRes_t)value;
}

ina219_shuntRes_t INA219::getShuntRes(void)
{
    unsigned int value;

    value = readRegister16(INA219_REG_CONFIG);
    value &= 0x0078;
    value >>= 3;

    return (ina219_shuntRes_t)value;
}

*/



