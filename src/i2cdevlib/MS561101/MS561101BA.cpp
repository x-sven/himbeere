// I2Cdev library collection - MS5611-01BA01 I2C device class header file
// Based on MEAS MS5611-01BA01 datasheet, 07/2011 (DA5611-01BA01_006)
// 6/12/2012 by Manuel J Guisado <mjguisado@gmail.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Modified by Sven (someone@online.de) to be applied for Raspberry Pi
// Changelog:
//     2012-06-12 - Initial release (Manuel)
//     2012-02-10 - ReadWord(s) calls replaced by readBytes to prepare byte swapping for x86 (Sven)

/* ============================================
 I2Cdev device library code is placed under the MIT license
 Copyright (c) 2012 Manuel J Guisado

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ===============================================
 */

#include "MS561101BA.h"

#ifndef ARDUINO
#include <cstdio>
#include <inttypes.h>
#include <cmath>
#define boolean bool
#include "Delay.h"
#endif

/** Default constructor, uses default I2C address and
 * default oversample rate.
 * @see MS561101BA_DEFAULT_ADDRESS
 * @see MS561101BA_OSR_1024
 */
//MS561101BA::MS561101BA()
//{
//    devAddr = MS561101BA_DEFAULT_ADDRESS;
//    defaultOsr = MS561101BA_OSR_1024;
//}

/** Specific address constructor.
 * @param address I2C address
 * @see MS561101BA_ADDRESS_CSB_LOW
 * @see MS561101BA_ADDRESS_CSB_HIGH
 * @see MS561101BA_DEFAULT_ADDRESS
 * @see MS561101BA_OSR_1024
 */
//MS561101BA::MS561101BA(uint8_t address)
//{
//    devAddr = address;
//    defaultOsr = MS561101BA_OSR_1024;
//}

/** Specific address and oversample rate constructor.
 * @param address I2C address
 * @param osr oversample rate
 * @see MS561101BA_ADDRESS_CSB_LOW
 * @see MS561101BA_ADDRESS_CSB_HIGH
 * @see MS561101BA_DEFAULT_ADDRESS
 * @see MS561101BA_OSR_1024
 */
MS561101BA::MS561101BA(uint8_t address, uint8_t osr)
{
    devAddr = address;
    defaultOsr = osr;
}
/** Power on and prepare for general usage.
 */
void MS561101BA::initialize()
{
    if (reset())
    {
        delay(100); // Time after reset, otherwise PROM read fails
        readPROM();
    }
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool MS561101BA::testConnection()
{
    uint8_t buffer[2] = {0,0};
    boolean retval = false;
    //return I2Cdev::readWord(devAddr, MS561101BA_RA_PROM, &aux);
    retval = I2Cdev::readBytes(devAddr, MS561101BA_RA_PROM, 2, buffer);
    return retval;
}

/** Send a reset command.
 * The Reset sequence shall be sent once after power-on to make
 * sure that the calibration PROM gets loaded into the internal
 * register. It can be also used to reset the device ROM from
 * an unknown condition.
 * @return Status of operation (true = success)
 */
bool MS561101BA::reset()
{
    m_d1 = 0;
    m_d2 = 0;
    return I2Cdev::writeBytes(devAddr, MS561101BA_RA_RESET, 0, NULL);
}

/**
 * Read read the content of the calibration PROM.
 * The read command for PROM shall be executed once after reset
 * by the user to calculate the calibration coefficients.
 * @return Status of operation (true = success)
 */
bool MS561101BA::readPROM()
{
    boolean success = true;
    uint8_t buffer[2] = {0,0};
    for (uint8_t add = 0; success && add < MS561101BA_PROM_NUM_REGISTERS; add++)
    {
        uint8_t regAddr = MS561101BA_RA_PROM + (add << 1);
        //success = I2Cdev::readWord(devAddr, regAddr, &prom[add]);
        success = I2Cdev::readBytes(devAddr, regAddr, 2, buffer);
        prom[add] = (((int16_t)buffer[0]) << 8) | buffer[1];
    }
    return success;
}

/**
 * Set the default oversample rate (osr)
 * @param osr Oversample rate. Optional.
 * @return Status of operation (true = success)
 */
bool MS561101BA::setOverSampleRate(uint8_t osr)
{
    if (osr == MS561101BA_OSR_256
            || osr == MS561101BA_OSR_512
            || osr == MS561101BA_OSR_1024
            || osr == MS561101BA_OSR_2048
            || osr == MS561101BA_OSR_4096)
    {
        defaultOsr = osr;
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Read the pressure and temperature.
 * This method use the second order temperature
 * compensation algorithm described in the datasheet.
 * @param pressure Pressure pointer.
 * @param temperature Temperature pointer.
 * @param osr Oversample rate. Optional.
 * @return Status of operation (true = success)
 */
bool MS561101BA::read(int32_t * pressure, int32_t * temperature)
{

    int32_t d1 = readD1(defaultOsr); //pressure
    int32_t d2 = readD2(defaultOsr); //temperature
    return( TemperatureCompensation(pressure,temperature, d1, d2) );
}

uint16_t MS561101BA::StartPressureConversion(void)
{
    startConversion(MS561101BA_RA_D1, defaultOsr);
    return getConversionTime();
}
uint16_t MS561101BA::StartTemperatureConversion(void)
{
    startConversion(MS561101BA_RA_D2, defaultOsr);
    return getConversionTime();
}

void MS561101BA::ReadPressure(void)
{
    m_d1 = readConversion(MS561101BA_RA_D1);
}

void MS561101BA::ReadTemperature(void)
{
    m_d2 = readConversion(MS561101BA_RA_D2);
}

void MS561101BA::getValues(int32_t *pressure, int32_t *temperature)
{
    if( !TemperatureCompensation(pressure, temperature, m_d1, m_d2) )
    {
        //error
        *pressure = 0;
        *temperature = 0;
    }
}

bool MS561101BA::TemperatureCompensation(int32_t * pressure, int32_t * temperature, int32_t d1, int32_t d2)
{
    if (d1 < 0 || d2 < 0) return false;

    int32_t dT = d2 - (((uint32_t) getTREF()) << 8);
    double t = 2000.0 + ((int64_t) dT) * getTEMPSENS() / POW_2_23;

    double off  = (((int64_t) getOFFT1())  << 16) +
                  ((int64_t) dT) * getTCO() / POW_2_7;
    double sens = (((int64_t) getSENST1()) << 15) +
                  ((int64_t) dT) * getTCS() / POW_2_8;

    // Second order temperature compensation
    if (t < 2000)
    {
        double square = pow (dT,2);
        double t2 = square / POW_2_31;
        square = pow (t-2000,2);
        double off2  = square * 5 / 2;
        double sens2 = square * 5 / 4;
        if (t < 15)
        {
            square = pow(t+1500,2);
            off2  += square * 7;
            sens2 += square * 11 / 2;
        }
        t    -= t2;
        off  -= off2;
        sens -= sens2;
    }

    double p = ((sens * d1 / POW_2_21) - off) / POW_2_15;
    *pressure    = (int32_t) p;
    *temperature = (int32_t) t;

    return true;

}

/**
 * Read the content of the D1 register (pressure).
 * @param osr Oversample rate. Optional.
 * @return content of the D1 register. -1 in case of error.
 */
int32_t MS561101BA::readD1(int8_t osr)
{
    if (osr == -1) osr = defaultOsr;
    return readConversion(MS561101BA_RA_D1, osr);
}

/**
 * Read the content of the D2 register (temperature).
 * @param osr Oversample rate. Optional.
 * @return content of the D1 register. -1 in case of error.
 */
int32_t MS561101BA::readD2(int8_t osr)
{
    if (osr == -1) osr = defaultOsr;
    return readConversion(MS561101BA_RA_D2, osr);
}

uint16_t MS561101BA::getConversionTime(void)
{
    uint16_t maxConversionTime;
    switch (defaultOsr)
    {
    case MS561101BA_OSR_256:
        maxConversionTime = MS561101BA_MAX_CONVERSION_TIME_OSR_256;
        break;
    case MS561101BA_OSR_512:
        maxConversionTime = MS561101BA_MAX_CONVERSION_TIME_OSR_512;
        break;
    case MS561101BA_OSR_1024:
        maxConversionTime = MS561101BA_MAX_CONVERSION_TIME_OSR_1024;
        break;
    case MS561101BA_OSR_2048:
        maxConversionTime = MS561101BA_MAX_CONVERSION_TIME_OSR_2048;
        break;
    case MS561101BA_OSR_4096:
        maxConversionTime = MS561101BA_MAX_CONVERSION_TIME_OSR_4096;
        break;
    default:
        maxConversionTime = 0;
    }
    return( maxConversionTime );
}

int32_t MS561101BA::readConversion(uint8_t regAddr, uint8_t osr)
{
    startConversion(regAddr, osr);
    delayMicroseconds( getConversionTime() );
    return( readConversion(regAddr) );
}

void MS561101BA::startConversion(uint8_t regAddr, uint8_t osr)
{
    I2Cdev::writeBytes(devAddr, regAddr + osr, 0, NULL);
}

int32_t MS561101BA::readConversion(uint8_t regAddr)
{
    uint8_t adcOutput[3];
    if (I2Cdev::readBytes(devAddr, 0, 3, adcOutput))
    {
        return (((int32_t) adcOutput[0]) << 16) | (((int32_t) adcOutput[1]) << 8)  | ((int32_t) adcOutput[2]);
    }
    return -1;
}

uint16_t MS561101BA::getSENST1()
{
    return prom[1];
}
uint16_t MS561101BA::getOFFT1()
{
    return prom[2];
}
uint16_t MS561101BA::getTCS()
{
    return prom[3];
}
uint16_t MS561101BA::getTCO()
{
    return prom[4];
}
uint16_t MS561101BA::getTREF()
{
    return prom[5];
}
uint16_t MS561101BA::getTEMPSENS()
{
    return prom[6];
}
