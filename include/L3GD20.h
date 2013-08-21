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

#ifndef L3GD20_H_
#define L3GD20_H_

#include "ch.h"
#include "hal.h"

class L3GD20
{
	public:
		L3GD20(I2CDriver *i2cp);

		uint8_t setSELState(uint8_t SELState);
		void enableDefault() const;
		void readGyro(int16_t *pX, int16_t *pY, int16_t *pZ) const;
		void readTemperature(int8_t *temperature) const;
		void setFullScaleRange(uint8_t fullScaleRange) const;
		void setBandwidth(uint8_t bandwidth) const;
		void setOutputDataRate(uint8_t dataRate) const;

	protected:
		bool writeReg(uint8_t reg, uint8_t value) const;
		bool readReg(uint8_t reg, uint8_t *value) const;

	protected:
		I2CDriver * const i2cp;
		i2caddr_t i2cAddr;
};

#endif /* L3GD20_H_ */
