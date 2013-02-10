/*
 *  Copyright (C) 2013 Xo Wang
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL XO
 *  WANG BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 *  AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  Except as contained in this notice, the name of Xo Wang shall not be
 *  used in advertising or otherwise to promote the sale, use or other dealings
 *  in this Software without prior written authorization from Xo Wang.
 */

#ifndef VNH5050A_H_
#define VNH5050A_H_

#include "ch.h"
#include "hal.h"

class VNH5050A {
public:
    VNH5050A(
            PWMDriver *pwmp,
            pwmchannel_t channel,
            GPIO_TypeDef *port1,
            uint16_t pad1,
            GPIO_TypeDef *port2,
            uint16_t pad2);

    void setSpeed(int32_t speed) {
        if (speed == 0) {
            palSetPad(port1, pad1);
            palSetPad(port2, pad2);
            pwmEnableChannel(pwmp, channel, 0);
        } else if (speed < 0) {
            palClearPad(port1, pad1);
            palSetPad(port2, pad2);
            pwmEnableChannel(pwmp, channel, -speed);
        } else {
            palSetPad(port1, pad1);
            palClearPad(port2, pad2);
            pwmEnableChannel(pwmp, channel, speed);
        }
    }

    pwmcnt_t getRange() {
        return pwmp->period;
    }

protected:
    PWMDriver * const pwmp;
    const pwmchannel_t channel;
    GPIO_TypeDef * const port1;
    const uint16_t pad1;
    GPIO_TypeDef * const port2;
    const uint16_t pad2;
};

#endif /* VNH5050A_H_ */
