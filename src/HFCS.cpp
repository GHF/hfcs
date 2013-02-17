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

#include <algorithm>

HFCS *HFCS::instance = NULL;
icucnt_t HFCS::negativeWidth = 0;
icucnt_t HFCS::positiveWidth = 0;

HFCS::HFCS(A4960 &m1, VNH5050A &mLeft, VNH5050A &mRight, ICUDriver *icup) :
                m1(m1),
                mLeft(mLeft),
                mRight(mRight),
                icup(icup),
                pulseWidths { },
                currentPulse(0),
                channels { },
                channelsValid(false),
                lastValidChannels(0) {
}

static constexpr size_t LOOP_DELAY_US = 10000;
static constexpr systime_t LOOP_DELAY = US2ST(LOOP_DELAY_US);

NORETURN void HFCS::fastLoop() {
    icuEnable(icup);
    m1.setMode(true);

    systime_t ticks = chTimeNow();
    while (true) {
        if (channelsValid) {
            constexpr int32_t INPUT_LOW = 1200;
            constexpr int32_t INPUT_HIGH = 1800;
            constexpr int32_t DEADBAND = 17;

            const int32_t DC_OUTPUT_RANGE = mLeft.getRange();
            const int32_t aileron = mapRanges(INPUT_LOW, INPUT_HIGH, channels[0], -DC_OUTPUT_RANGE, DC_OUTPUT_RANGE, DEADBAND);
            const int32_t elevator = mapRanges(INPUT_LOW, INPUT_HIGH, channels[1], -DC_OUTPUT_RANGE, DC_OUTPUT_RANGE, DEADBAND);

            const int32_t left = std::min(std::max(elevator - aileron, -DC_OUTPUT_RANGE), DC_OUTPUT_RANGE);
            const int32_t right = std::min(std::max(elevator + aileron, -DC_OUTPUT_RANGE), DC_OUTPUT_RANGE);
            mLeft.setSpeed(left);
            mRight.setSpeed(right);

            const int32_t throttle = mapRanges(INPUT_LOW, INPUT_HIGH, channels[2], 0, m1.getRange(), 0);
            m1.setWidth(throttle);
        } else {
            m1.setWidth(0);
            mLeft.setSpeed(0);
            mRight.setSpeed(0);
        }

        ticks += LOOP_DELAY;
        chThdSleepUntil(ticks);
    }
}

NORETURN void HFCS::failsafeLoop() {
    while (true) {
        if (chTimeNow() - lastValidChannels > MS2ST(500)) {
            channelsValid = false;
            palClearPad(GPIOA, GPIOA_LEDQ);
        }

        if (palReadPad(GPIOC, GPIOC_M1_DIAG) == 0) {
            m1.setMode(false);
        }

        chThdSleepMilliseconds(200);
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

static inline int32_t nabs(int32_t i) {
    return i < 0 ? i : -i;
}

static inline int32_t avg(int32_t a, int32_t b) {
    return (a + b) / 2;
}

static inline int32_t signum(int32_t i) {
    return (i > 0) - (i < 0);
}

int32_t HFCS::mapRanges(int32_t inLow, int32_t inHigh, int32_t inValue, int32_t outLow, int32_t outHigh, int32_t deadband) {
    const int32_t inCenter = avg(inLow, inHigh);
    int32_t centeredInput = inValue - inCenter;
    if (nabs(centeredInput) > -deadband) {
        centeredInput = 0;
    } else {
        centeredInput -= signum(centeredInput) * deadband;
    }

    const int32_t inScale = inHigh - inLow - 2 * deadband;
    const int32_t outScale = outHigh - outLow;
    const int32_t outCenter = avg(outLow, outHigh);

    const int32_t outValue = centeredInput * outScale / inScale + outCenter;

    return std::max(outLow, std::min(outHigh, outValue));
}

