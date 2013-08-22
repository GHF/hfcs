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

#include "ch.h"
#include "hal.h"

#include "HFCS.h"
#include "A4960.h"
#include "VNH5050A.h"
#include "L3GD20.h"

#include <algorithm>

HFCS *HFCS::instance = NULL;
icucnt_t HFCS::negativeWidth = 0;
icucnt_t HFCS::positiveWidth = 0;

HFCS::HFCS(A4960 &m1, VNH5050A &mLeft, VNH5050A &mRight, ICUDriver *icup, L3GD20 &gyro) :
                m1(m1),
                mLeft(mLeft),
                mRight(mRight),
                icup(icup),
                gyro(gyro),
                pulseWidths { },
                currentPulse(0),
                channels { },
                channelsValid(false),
                lastValidChannels(0),
                gyroEnable(false) {
}

static constexpr size_t LOOP_DELAY_US = 10000;
static constexpr systime_t LOOP_DELAY = US2ST(LOOP_DELAY_US);

NORETURN void HFCS::fastLoop() {
    icuEnable(icup);
    m1.setMode(true);

    systime_t ticks = chTimeNow();
    while (true) {
        palSetPad(GPIOA, GPIOA_LEDR);
        if (channelsValid) {
            if (gyroEnable) {
                gyroMotorControl();
            } else {
                manualMotorControl();
            }
        } else {
            disableMotors();
        }

        ticks += LOOP_DELAY;
        palClearPad(GPIOA, GPIOA_LEDR);
        chThdSleepUntil(ticks);
    }
}

inline void HFCS::gyroMotorControl() {
    // todo: implement PID control for z-axis angular rate
    mLeft.setSpeed(0);
    mRight.setSpeed(0);

    const int32_t throttle = mapRanges(INPUT_LOW, INPUT_HIGH, channels[2], 0, m1.getRange(), 0);
    m1.setWidth(throttle);
}

inline void HFCS::manualMotorControl() {
    const int32_t dcOutRange = mLeft.getRange();
    const int32_t aileron = mapRanges(INPUT_LOW, INPUT_HIGH, channels[0], -dcOutRange, dcOutRange, DEADBAND);
    const int32_t elevator = mapRanges(INPUT_LOW, INPUT_HIGH, channels[1], -dcOutRange, dcOutRange, DEADBAND);

    const int32_t left = std::min(std::max(elevator - aileron, -dcOutRange), dcOutRange);
    const int32_t right = std::min(std::max(elevator + aileron, -dcOutRange), dcOutRange);
    mLeft.setSpeed(left);
    mRight.setSpeed(right);

    const int32_t throttle = mapRanges(INPUT_LOW, INPUT_HIGH, channels[2], 0, m1.getRange(), 0);
    m1.setWidth(throttle);
}

inline void HFCS::disableMotors() {
    m1.setWidth(0);
    mLeft.setSpeed(0);
    mRight.setSpeed(0);
}

NORETURN void HFCS::failsafeLoop() {
    while (true) {
        if (chTimeNow() - lastValidChannels > MS2ST(500)) {
            channelsValid = false;
            palClearPad(GPIOA, GPIOA_LEDQ);
        }

        if (palReadPad(GPIOC, GPIOC_M1_DIAG) == 0) {
//            m1.setMode(false);
        }

        chThdSleepMilliseconds(50);
    }
}

void HFCS::newPulse() {
    // start of pulse train (long low time)
    if (negativeWidth > 5000) {
        currentPulse = 0;
        if (checkPulseWidth(positiveWidth)) {
            pulseWidths[0] = positiveWidth;
            currentPulse++;
        }
    } else {
        if (checkPulseWidth(negativeWidth) && checkPulseWidth(positiveWidth)) {
            pulseWidths[currentPulse++] = negativeWidth;
            pulseWidths[currentPulse++] = positiveWidth;
            if (currentPulse >= NUM_CHANNELS) {
                currentPulse = 0;
                std::copy(pulseWidths, pulseWidths + NUM_CHANNELS, channels);
                palTogglePad(GPIOA, GPIOA_LEDQ);
                lastValidChannels = chTimeNow();
                channelsValid = true;
            }
        } else {
            currentPulse = 0;
        }
    }
}

void HFCS::icuWidthCb(ICUDriver *icup) {
    negativeWidth = icuGetWidthI(icup);
}

void HFCS::icuPeriodCb(ICUDriver *icup) {
    positiveWidth = icuGetPeriodI(icup) - negativeWidth;
    instance->newPulse();
}

/**
 * Negative absolute value. Used to avoid undefined behavior for most negative
 * integer (see C99 standard 7.20.6.1.2 and footnote 265 for the description of
 * abs/labs/llabs behavior).
 *
 * @param i input value, as a 32-bit signed integer
 * @return negative absolute value of i; defined for all values of i
 */
static inline int32_t nabs(int32_t i) {
#if (((int32_t)-1) >> 1) == ((int32_t)-1)
    // signed right shift sign-extends (arithmetic)
    const int32_t negSign = ~(i >> 31); // splat sign bit into all 32 and complement
    // if i is positive (negSign is -1), xor will invert i and sub will add 1
    // otherwise i is unchanged
    return (i ^ negSign) - negSign;
#else
    return i < 0 ? i : -i;
#endif
}

/**
 * Overflow-safe average of two signed integers. The naive average function is
 * erroneous when the sum of the inputs overflows integer limits; this average
 * works by summing the halves of the input values and then correcting the sum
 * for rounding.
 *
 * @param a first value, as a 32-bit signed integer
 * @param b second value, as a 32-bit signed integer
 * @return signed average of the two values, rounded towards zero if their
 * average is not an integer
 */
static inline int32_t avg(int32_t a, int32_t b) {
#if (((int32_t)-1) >> 1) != ((int32_t)-1)
#error "Arithmetic right shift not available with this compiler/platform."
#endif
    // shifts divide by two, rounded towards negative infinity
    const int32_t sumHalves = (a >> 1) + (b >> 1);
    // this has error of magnitude one if both are odd
    const uint32_t bothOdd = (a & b) & 1;
    // round toward zero; add one if one input is odd and sum is negative
    const uint32_t roundToZero = (sumHalves < 0) & (a ^ b);

    // result is sum of halves corrected for rounding
    return sumHalves + bothOdd + roundToZero;
}

static inline int32_t signum(int32_t i) {
    return (i > 0) - (i < 0);
}

/**
 * Proportionally map one range of values to another, with deadband. Care must
 * be taken to not exceed integer limits in this computation!
 *
 * todo: describe exactly conditions for integer overflow errors
 *
 * @param inLow input range lower bound. inValue can not be less than this.
 * @param inHigh input range upper bound. inValue can not exceed this.
 * @param inValue input value to map
 * @param outLow output range lower bound
 * @param outHigh output range upper bound
 * @param deadband distance from center of input range for which all input
 *  values are mapped to zero
 * @return mapped input value after adjusting for deadband
 */
int32_t HFCS::mapRanges(int32_t inLow, int32_t inHigh, int32_t inValue, int32_t outLow, int32_t outHigh, int32_t deadband) {
    // center the input range on zero
    const int32_t inCenter = avg(inLow, inHigh);
    int32_t centeredInput = inValue - inCenter;

    // cut away the deadband from the centered input
    if (nabs(centeredInput) > -deadband) {
        centeredInput = 0;
    } else {
        centeredInput -= signum(centeredInput) * deadband;
    }

    // compute the length of input and output ranges
    const int32_t inScale = inHigh - inLow - 2 * deadband;
    const int32_t outScale = outHigh - outLow;
    const int32_t outCenter = avg(outLow, outHigh);

    // scale by output to input ratio, then shift from zero into range
    // WARNING: this computation can easily overflow!
    const int32_t outValue = centeredInput * outScale / inScale + outCenter;

    // clamp output within range
    return std::max(outLow, std::min(outHigh, outValue));
}
