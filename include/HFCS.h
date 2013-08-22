/*
 *  Copyright (C) 2012-2013 Xo Wang
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

#ifndef HFCS_H_
#define HFCS_H_

#define NORETURN __attribute__((noreturn))
#define ALIGNED(x) __attribute__((aligned(x)))

#define M1_SPI (SPID2)

#define M1_PWM_FREQ 20000
#define M1_PWM_PERIOD (STM32_TIMCLK1 / M1_PWM_FREQ)

#define M1_PWM (PWMD3)
#define M1_PWM_CHAN (3)

#define DC_PWM_FREQ 17000
#define DC_PWM_PERIOD (STM32_TIMCLK1 / DC_PWM_FREQ)

#define DC_PWM (PWMD1)
#define DC_PWM_AB_CHAN (0)
#define DC_PWM_XY_CHAN (3)

#define PPM_ICU (ICUD2)

#define GYRO_I2C (I2CD1)

#define DBG_SERIAL (SD6)

class A4960;
class VNH5050A;
struct ICUDriver;
class L3GD20;

#include "Pid.hpp"

class HFCS {
public:
    HFCS(A4960 &m1, VNH5050A &mLeft, VNH5050A &mRight, ICUDriver *icup, L3GD20 &gyro);

    void init();
    NORETURN void fastLoop();
    NORETURN void failsafeLoop();

    static HFCS *instance;
    static void icuWidthCb(ICUDriver *icup);
    static void icuPeriodCb(ICUDriver *icup);

protected:
    A4960 &m1;
    VNH5050A &mLeft;
    VNH5050A &mRight;
    ICUDriver * const icup;
    L3GD20 &gyro;

    static constexpr size_t NUM_CHANNELS = 5;
    icucnt_t pulseWidths[NUM_CHANNELS];
    size_t currentPulse;
    const int32_t dcOutRange;

    int32_t channels[NUM_CHANNELS];
    bool channelsValid;
    systime_t lastValidChannels;

    bool gyroEnable;
    PidNs::Pid<float, float> gyroPID;

    static constexpr int32_t INPUT_LOW = 1200;
    static constexpr int32_t INPUT_HIGH = 1800;
    static constexpr int32_t DEADBAND = 17;
    static icucnt_t negativeWidth;
    static icucnt_t positiveWidth;

    void newPulse();

    void gyroMotorControl();
    void manualMotorControl();
    void disableMotors();

    bool checkPulseWidth(icucnt_t pulseWidth) const {
        if (pulseWidth > 2200 || pulseWidth < 800) {
            return false;
        }
        return true;
    }

    static int32_t mapRanges(int32_t inLow, int32_t inHigh, int32_t inValue, int32_t outLow, int32_t outHigh, int32_t deadband);
};

#endif /* HFCS_H_ */
