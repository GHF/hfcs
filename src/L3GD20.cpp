/*
 * Copyright (c) 2013 Xo Wang modifications for ChibiOS on STM32
 * Copyright (c) 2012 Pololu Corporation.  For more information, see
 *
 * http://www.pololu.com/
 * http://forum.pololu.com/
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "L3GD20.h"

// Defines /////////////////////////////////////////////////////////////////////

#define _MULTI_REGISTER_GYRO_READ

// Constants ///////////////////////////////////////////////////////////////////

// 7-bit address; the last bit is set based on reads and writes
#define L3GD20_ADDR_SEL_LOW     (0x6a)
#define L3GD20_ADDR_SEL_HIGH    (0x6b)

static const systime_t I2C_TIMEOUT = MS2ST(4);

// register addresses

#define L3GD20_WHO_AM_I      0x0F

#define L3GD20_CTRL_REG1     0x20
#define L3GD20_CTRL_REG2     0x21
#define L3GD20_CTRL_REG3     0x22
#define L3GD20_CTRL_REG4     0x23
#define L3GD20_CTRL_REG5     0x24
#define L3GD20_REFERENCE     0x25
#define L3GD20_OUT_TEMP      0x26
#define L3GD20_STATUS_REG    0x27

#define L3GD20_OUT_X_L       0x28
#define L3GD20_OUT_X_H       0x29
#define L3GD20_OUT_Y_L       0x2A
#define L3GD20_OUT_Y_H       0x2B
#define L3GD20_OUT_Z_L       0x2C
#define L3GD20_OUT_Z_H       0x2D

#define L3GD20_FIFO_CTRL_REG 0x2E
#define L3GD20_FIFO_SRC_REG  0x2F

#define L3GD20_INT1_CFG      0x30
#define L3GD20_INT1_SRC      0x31
#define L3GD20_INT1_THS_XH   0x32
#define L3GD20_INT1_THS_XL   0x33
#define L3GD20_INT1_THS_YH   0x34
#define L3GD20_INT1_THS_YL   0x35
#define L3GD20_INT1_THS_ZH   0x36
#define L3GD20_INT1_THS_ZL   0x37
#define L3GD20_INT1_DURATION 0x38

// Public Methods //////////////////////////////////////////////////////////////

L3GD20::L3GD20(I2CDriver *i2cp) :
        i2cp(i2cp), i2cAddr(L3GD20_ADDR_SEL_HIGH) {
}

uint8_t L3GD20::setSELState(uint8_t SELState) {
    uint8_t returnValue = 0;

    if (0 == SELState) {
        i2cAddr = L3GD20_ADDR_SEL_LOW;
    } else if (1 == SELState) {
        i2cAddr = L3GD20_ADDR_SEL_HIGH;
    }

    return returnValue;
}

// Turns on the L3GD20's gyro and places it in normal mode.
void L3GD20::enableDefault() const {
    // 0x0F = 0b00001111
    // Normal power mode, all axes enabled
    const bool status = writeReg(L3GD20_CTRL_REG1, 0x0f);
}

// Writes a gyro register
bool L3GD20::writeReg(uint8_t reg, uint8_t value) const {
    const uint8_t txbuf[2] = { reg, value };

    i2cAcquireBus (i2cp);
    const msg_t status = i2cMasterTransmitTimeout(i2cp, i2cAddr, txbuf, 2, nullptr, 0, I2C_TIMEOUT);
    i2cReleaseBus(i2cp);
    return (status == RDY_OK);
}

// Reads a gyro register
bool L3GD20::readReg(uint8_t reg, uint8_t *value) const {
    i2cAcquireBus (i2cp);
    const msg_t status = i2cMasterTransmitTimeout(i2cp, i2cAddr, &reg, 1, value, 1, I2C_TIMEOUT);
    i2cReleaseBus(i2cp);

    return (status == RDY_OK);
}

// Reads the 3 gyro channels
void L3GD20::readGyro(int16_t *pX, int16_t *pY, int16_t *pZ) const {
    uint8_t values[6];

#ifdef _MULTI_REGISTER_GYRO_READ
    // assert MSB of address so gyro auto-increments slave-transmit subaddress
    const uint8_t reg = L3GD20_OUT_X_L | (1 << 7);

    i2cAcquireBus (i2cp);
    const msg_t status = i2cMasterTransmitTimeout(i2cp, i2cAddr, &reg, 1, values, 6, I2C_TIMEOUT);
    i2cReleaseBus(i2cp);
#else
    static const uint8_t regs[6] = {
                L3GD20_OUT_X_L,
                L3GD20_OUT_X_H,
                L3GD20_OUT_Y_L,
                L3GD20_OUT_Y_H,
                L3GD20_OUT_Z_L,
                L3GD20_OUT_Z_H };

    for (size_t i = 0; i < 6; i++) {
        const msg_t status = readReg(regs[i], &values[i]);
    }
#endif	// _MULTI_REGISTER_GYRO_READ
    *pX = int16_t(values[1] << 8 | values[0]);
    *pY = int16_t(values[3] << 8 | values[2]);
    *pZ = int16_t(values[5] << 8 | values[4]);
}

void L3GD20::readTemperature(int8_t *temperature) const {
    uint8_t registerValue;

    const bool status = readReg(L3GD20_OUT_TEMP, &registerValue);

    *temperature = (int8_t) registerValue;
}

void L3GD20::setFullScaleRange(uint8_t fullScaleRange) const {
    uint8_t registerValue;

    const bool status = readReg(L3GD20_CTRL_REG4, &registerValue);

    registerValue &= ~(0x30);
    registerValue |= ((fullScaleRange & 0x03) << 4);

    writeReg(L3GD20_CTRL_REG4, registerValue);
}

void L3GD20::setBandwidth(uint8_t bandwidth) const {
    uint8_t registerValue;

    const bool status = readReg(L3GD20_CTRL_REG1, &registerValue);

    registerValue &= ~(0x30);
    registerValue |= ((bandwidth & 0x03) << 4);

    writeReg(L3GD20_CTRL_REG1, registerValue);
}

void L3GD20::setOutputDataRate(uint8_t dataRate) const {
    uint8_t registerValue;

    const bool status = readReg(L3GD20_CTRL_REG1, &registerValue);

    registerValue &= ~(0xC0);
    registerValue |= ((dataRate & 0x03) << 6);

    writeReg(L3GD20_CTRL_REG1, registerValue);
}

