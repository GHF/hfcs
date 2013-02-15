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
            constexpr int32_t INPUT_RANGE = 300;
            const int32_t DC_OUTPUT_RANGE = mLeft.getRange();
            int32_t aileron = (channels[0] * DC_OUTPUT_RANGE) / INPUT_RANGE;
            int32_t elevator = (channels[1] * DC_OUTPUT_RANGE) / INPUT_RANGE;
            aileron = std::min(std::max(aileron, -DC_OUTPUT_RANGE), DC_OUTPUT_RANGE);
            elevator = std::min(std::max(elevator, -DC_OUTPUT_RANGE), DC_OUTPUT_RANGE);
            mLeft.setSpeed(elevator - aileron);
            mRight.setSpeed(elevator + aileron);

            const int32_t M1_OUTPUT_RANGE = m1.getRange();
            int32_t throttle = ((channels[2] + INPUT_RANGE) * M1_OUTPUT_RANGE) / (INPUT_RANGE * 2);
            throttle = std::min(std::max(throttle, 0L), M1_OUTPUT_RANGE);
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
                for (size_t i = 0; i < NUM_CHANNELS; i++) {
                    channels[i] = pulseWidths[i] - 1500;
                }
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
