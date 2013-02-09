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

#ifndef TORTILLA_H_
#define TORTILLA_H_

#define NORETURN __attribute__((noreturn))
#define ALIGNED(x) __attribute__((aligned(x)))

#define M1_SPI (SPID2)

#define PWM_FREQ 20000
#define PWM_PERIOD (STM32_TIMCLK1 / PWM_FREQ)

#define DC_PWM (PWMD1)
#define DC_PWM_AB_CHAN (0)
#define DC_PWM_XY_CHAN (3)

#define M1_PWM (PWMD3)
#define M1_PWM_CHAN (3)

#define DBG_SERIAL (SD6)

#endif /* TORTILLA_H_ */
