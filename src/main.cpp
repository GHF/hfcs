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

#include "ch.h"
#include "hal.h"

#include "HFCS.h"
#include "A4960.h"
#include "VNH5050A.h"
#include "L3GD20.h"

// heartbeat thread
static WORKING_AREA(waHeartbeat, 128);
NORETURN static void threadHeartbeat(void *arg) {
    (void) arg;
    chRegSetThreadName("heartbeat");
    while (TRUE) {
        palClearPad(GPIOA, GPIOA_LEDP);
        chThdSleepMilliseconds(900);
        palSetPad(GPIOA, GPIOA_LEDP);
        chThdSleepMilliseconds(100);
    }
    chThdExit(0);
}

// failsafe thread
static WORKING_AREA(waFailsafe, 128);
NORETURN static void threadFailsafe(void *arg) {
    (void) arg;
    chRegSetThreadName("failsafe");
    static_cast<HFCS *>(arg)->failsafeLoop();
    chThdExit(0);
}

int main(void) {
    halInit();
    chSysInit();

    chThdSleepMilliseconds(200);

    // serial setup
    const SerialConfig dbgSerialConfig = { 115200, 0, USART_CR2_STOP1_BITS, USART_CR3_CTSE | USART_CR3_RTSE };
    sdStart(&DBG_SERIAL, &dbgSerialConfig);

    // VNH5050A PWM setup
    const PWMConfig dcPWMConfig = { STM32_TIMCLK1, DC_PWM_PERIOD, nullptr, {
            { PWM_OUTPUT_ACTIVE_HIGH, nullptr },
            { PWM_OUTPUT_DISABLED, nullptr },
            { PWM_OUTPUT_DISABLED, nullptr },
            { PWM_OUTPUT_ACTIVE_HIGH, nullptr } }, 0, };
    pwmStart(&DC_PWM, &dcPWMConfig);

    // DC motor setup
    VNH5050A dcAB(&DC_PWM, DC_PWM_AB_CHAN, GPIOC, GPIOC_MTR_A, GPIOA, GPIOA_MTR_B);
    VNH5050A dcXY(&DC_PWM, DC_PWM_XY_CHAN, GPIOA, GPIOA_MTR_X, GPIOA, GPIOA_MTR_Y);

    // A4960 PWM setup
    const PWMConfig mPWMConfig = { STM32_TIMCLK1, M1_PWM_PERIOD, nullptr, {
            { PWM_OUTPUT_DISABLED, nullptr },
            { PWM_OUTPUT_DISABLED, nullptr },
            { PWM_OUTPUT_DISABLED, nullptr },
            { PWM_OUTPUT_ACTIVE_HIGH, nullptr } }, 0, };
    pwmStart(&M1_PWM, &mPWMConfig);

    // SPI setup
    // speed = pclk/8 = 5.25MHz
    const SPIConfig m1SPIConfig = { NULL, GPIOC, GPIOC_M1_NSS, SPI_CR1_DFF | SPI_CR1_BR_1 };
    spiStart(&M1_SPI, &m1SPIConfig);

    // weapon motor setup
    A4960 m1(&M1_SPI, &M1_PWM, M1_PWM_CHAN);

    // input capture & high-res timer
    const ICUConfig icuConfig = { ICU_INPUT_ACTIVE_LOW, 1000000, HFCS::icuWidthCb, HFCS::icuPeriodCb };
    icuStart(&PPM_ICU, &icuConfig);

    // gyro I2C setup
    const I2CConfig i2cConfig = { OPMODE_I2C, 400000, FAST_DUTY_CYCLE_2 };
    i2cStart(&GYRO_I2C, &i2cConfig);

    // gyro setup
    L3GD20 gyro(&GYRO_I2C);
    gyro.setSlaveAddrLSB(1);
    gyro.enableDefault();
    gyro.setFullScaleRange(2); // 2000 dps
    gyro.setOutputDataRate(2); // 380 Hz
    gyro.setBandwidth(2); // 100 Hz cut-off

    // initialize control loop
    HFCS hfcs(m1, dcAB, dcXY, &PPM_ICU, gyro);
    HFCS::instance = &hfcs;

    // start slave threads
    chThdCreateStatic(waHeartbeat, sizeof(waHeartbeat), IDLEPRIO, tfunc_t(threadHeartbeat), nullptr);
    chThdCreateStatic(waFailsafe, sizeof(waFailsafe), LOWPRIO, tfunc_t(threadFailsafe), &hfcs);

    // done with setup
    palClearPad(GPIOA, GPIOA_LEDQ);
    palClearPad(GPIOA, GPIOA_LEDR);

    // start the fast loop
    hfcs.fastLoop();
}
