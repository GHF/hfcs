/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.
    Modifications (C) 2012-2013 Xo Wang

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for High Fructose board (based on STMicroelectronics STM32F4-Discovery board files).
 */

/*
 * Board identifier.
 */
#define BOARD_TORTILLA_V2_0
#define BOARD_NAME              "High Fructose v0.0 (C) 2013 Xo Wang"

/*
 * Board frequencies.
 * NOTE: The LSE crystal is not fitted by default on the board.
 */
#define STM32_LSECLK            0
#define STM32_HSECLK            8000000

/*
 * Board voltages.
 * Required for performance limits calculation.
 */
#define STM32_VDD               300

/*
 * MCU type as defined in the ST header file stm32f4xx.h.
 */
#define STM32F4XX

/*
 * IO pins assignments.
 */
#define GPIOA_LEDP              1
#define GPIOA_LEDQ              2
#define GPIOA_LEDR              3
#define GPIOA_PWM_AB            8
#define GPIOA_MTR_B             9
#define GPIOA_MTR_X             10
#define GPIOA_PWM_XY            11
#define GPIOA_MTR_Y             12
#define GPIOA_SWDIO             13
#define GPIOA_SWCLK             14
#define GPIOA_RX_PPM            15

#define GPIOB_M1_PWM            1
#define GPIOB_SWO               3
#define GPIOB_GYRO_SDA          7
#define GPIOB_GYRO_SCL          8
#define GPIOB_M1_TACHO          11
#define GPIOB_MTR_EN            12
#define GPIOB_M1_SCK            13
#define GPIOB_M1_MISO           14
#define GPIOB_M1_MOSI           15

#define GPIOC_M1_DIAG           5
#define GPIOC_UART_TX           6
#define GPIOC_UART_RX           7
#define GPIOC_M1_NSS            8
#define GPIOC_MTR_A             9
#define GPIOC_BUT1              13

#define GPIOD_OSC_IN            0
#define GPIOD_OSC_OUT           1

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 * Please refer to the STM32 Reference Manual for details.
 */
#define PIN_MODE_INPUT(n)           (0U << ((n) * 2))
#define PIN_MODE_OUTPUT(n)          (1U << ((n) * 2))
#define PIN_MODE_ALTERNATE(n)       (2U << ((n) * 2))
#define PIN_MODE_ANALOG(n)          (3U << ((n) * 2))
#define PIN_OTYPE_PUSHPULL(n)       (0U << (n))
#define PIN_OTYPE_OPENDRAIN(n)      (1U << (n))
#define PIN_OSPEED_2M(n)            (0U << ((n) * 2))
#define PIN_OSPEED_25M(n)           (1U << ((n) * 2))
#define PIN_OSPEED_50M(n)           (2U << ((n) * 2))
#define PIN_OSPEED_100M(n)          (3U << ((n) * 2))
#define PIN_PUDR_FLOATING(n)        (0U << ((n) * 2))
#define PIN_PUDR_PULLUP(n)          (1U << ((n) * 2))
#define PIN_PUDR_PULLDOWN(n)        (2U << ((n) * 2))
#define PIN_AFIO_AF(n, v)           ((v##U) << ((n % 8) * 4))

/*
 * Port A setup.
 */
#define VAL_GPIOA_MODER     (PIN_MODE_OUTPUT(0) |                           \
                             PIN_MODE_OUTPUT(GPIOA_LEDP) |                  \
                             PIN_MODE_OUTPUT(GPIOA_LEDQ) |                  \
                             PIN_MODE_OUTPUT(GPIOA_LEDR) |                  \
                             PIN_MODE_INPUT(4) |                            \
                             PIN_MODE_INPUT(5) |                            \
                             PIN_MODE_INPUT(6) |                            \
                             PIN_MODE_INPUT(7) |                            \
                             PIN_MODE_ALTERNATE(GPIOA_PWM_AB) |             \
                             PIN_MODE_OUTPUT(GPIOA_MTR_B) |                 \
                             PIN_MODE_OUTPUT(GPIOA_MTR_X) |                 \
                             PIN_MODE_ALTERNATE(GPIOA_PWM_XY) |             \
                             PIN_MODE_OUTPUT(GPIOA_MTR_Y) |                 \
                             PIN_MODE_ALTERNATE(GPIOA_SWDIO) |              \
                             PIN_MODE_ALTERNATE(GPIOA_SWCLK) |              \
                             PIN_MODE_ALTERNATE(GPIOA_RX_PPM))
#define VAL_GPIOA_OTYPER    0x00000000
#define VAL_GPIOA_OSPEEDR   0xFFFFFFFF
#define VAL_GPIOA_PUPDR     (PIN_PUDR_PULLUP(0) |                           \
                             PIN_PUDR_FLOATING(GPIOA_LEDP) |                \
                             PIN_PUDR_FLOATING(GPIOA_LEDQ) |                \
                             PIN_PUDR_FLOATING(GPIOA_LEDR) |                \
                             PIN_PUDR_PULLUP(4) |                           \
                             PIN_PUDR_PULLUP(5) |                           \
                             PIN_PUDR_PULLUP(6) |                           \
                             PIN_PUDR_PULLUP(7) |                           \
                             PIN_PUDR_FLOATING(GPIOA_PWM_AB) |              \
                             PIN_PUDR_FLOATING(GPIOA_MTR_B) |               \
                             PIN_PUDR_FLOATING(GPIOA_MTR_X) |               \
                             PIN_PUDR_FLOATING(GPIOA_PWM_XY) |              \
                             PIN_PUDR_FLOATING(GPIOA_MTR_Y) |               \
                             PIN_PUDR_PULLUP(GPIOA_SWDIO) |                 \
                             PIN_PUDR_PULLDOWN(GPIOA_SWCLK) |               \
                             PIN_PUDR_PULLDOWN(GPIOA_RX_PPM))
#define VAL_GPIOA_ODR       0xFFFFFFFF
#define VAL_GPIOA_AFRL      0x00000000
#define VAL_GPIOA_AFRH      (PIN_AFIO_AF(GPIOA_PWM_AB, 1) |                 \
                             PIN_AFIO_AF(GPIOA_PWM_XY, 1) |                 \
                             PIN_AFIO_AF(GPIOA_SWDIO, 0) |                  \
                             PIN_AFIO_AF(GPIOA_SWCLK, 0) |                  \
                             PIN_AFIO_AF(GPIOA_RX_PPM, 1))

/*
 * Port B setup.
 */
#define VAL_GPIOB_MODER     (PIN_MODE_INPUT(0) |                            \
                             PIN_MODE_ALTERNATE(GPIOB_M1_PWM) |             \
                             PIN_MODE_INPUT(2) |                            \
                             PIN_MODE_ALTERNATE(GPIOB_SWO) |                \
                             PIN_MODE_INPUT(4) |                            \
                             PIN_MODE_INPUT(5) |                            \
                             PIN_MODE_INPUT(6) |                            \
                             PIN_MODE_ALTERNATE(GPIOB_GYRO_SDA) |           \
                             PIN_MODE_ALTERNATE(GPIOB_GYRO_SCL) |           \
                             PIN_MODE_INPUT(9) |                            \
                             PIN_MODE_INPUT(10) |                           \
                             PIN_MODE_ALTERNATE(GPIOB_M1_TACHO) |           \
                             PIN_MODE_OUTPUT(GPIOB_MTR_EN) |                \
                             PIN_MODE_ALTERNATE(GPIOB_M1_SCK) |             \
                             PIN_MODE_ALTERNATE(GPIOB_M1_MISO) |            \
                             PIN_MODE_ALTERNATE(GPIOB_M1_MOSI))
#define VAL_GPIOB_OTYPER    (PIN_OTYPE_OPENDRAIN(GPIOB_GYRO_SDA) |          \
                             PIN_OTYPE_OPENDRAIN(GPIOB_GYRO_SCL) |          \
                             PIN_OTYPE_OPENDRAIN(GPIOB_MTR_EN))
#define VAL_GPIOB_OSPEEDR   0xFFFFFFFF
#define VAL_GPIOB_PUPDR     (PIN_PUDR_PULLUP(0) |                           \
                             PIN_PUDR_FLOATING(GPIOB_M1_PWM) |              \
                             PIN_PUDR_PULLUP(2) |                           \
                             PIN_PUDR_FLOATING(GPIOB_SWO) |                 \
                             PIN_PUDR_PULLUP(4) |                           \
                             PIN_PUDR_PULLUP(5) |                           \
                             PIN_PUDR_PULLUP(6) |                           \
                             PIN_PUDR_PULLUP(GPIOB_GYRO_SDA) |              \
                             PIN_PUDR_PULLUP(GPIOB_GYRO_SCL) |              \
                             PIN_PUDR_PULLUP(9) |                           \
                             PIN_PUDR_PULLUP(10) |                          \
                             PIN_PUDR_FLOATING(GPIOB_M1_TACHO) |            \
                             PIN_PUDR_PULLUP(GPIOB_MTR_EN) |                \
                             PIN_PUDR_FLOATING(GPIOB_M1_SCK) |              \
                             PIN_PUDR_FLOATING(GPIOB_M1_SCK) |              \
                             PIN_PUDR_FLOATING(GPIOB_M1_MOSI))
#define VAL_GPIOB_ODR       0xFFFFFFFF
#define VAL_GPIOB_AFRL      (PIN_AFIO_AF(GPIOB_M1_PWM, 2) |                 \
                             PIN_AFIO_AF(GPIOB_SWO, 0) |                    \
                             PIN_AFIO_AF(GPIOB_GYRO_SDA, 4))
#define VAL_GPIOB_AFRH      (PIN_AFIO_AF(GPIOB_GYRO_SCL, 4) |               \
                             PIN_AFIO_AF(GPIOB_M1_TACHO, 1) |               \
                             PIN_AFIO_AF(GPIOB_M1_SCK, 5) |                 \
                             PIN_AFIO_AF(GPIOB_M1_MISO, 5) |                \
                             PIN_AFIO_AF(GPIOB_M1_MOSI, 5))

/*
 * Port C setup.
 */
#define VAL_GPIOC_MODER     (PIN_MODE_INPUT(0) |                            \
                             PIN_MODE_INPUT(1) |                            \
                             PIN_MODE_INPUT(2) |                            \
                             PIN_MODE_INPUT(3) |                            \
                             PIN_MODE_INPUT(4) |                            \
                             PIN_MODE_INPUT(GPIOC_M1_DIAG) |                \
                             PIN_MODE_ALTERNATE(GPIOC_UART_TX) |            \
                             PIN_MODE_ALTERNATE(GPIOC_UART_RX) |            \
                             PIN_MODE_OUTPUT(GPIOC_M1_NSS) |                \
                             PIN_MODE_OUTPUT(GPIOC_MTR_A) |                 \
                             PIN_MODE_INPUT(10) |                           \
                             PIN_MODE_INPUT(11) |                           \
                             PIN_MODE_INPUT(12) |                           \
                             PIN_MODE_INPUT(GPIOC_BUT1) |                   \
                             PIN_MODE_INPUT(14) |                           \
                             PIN_MODE_INPUT(15))
#define VAL_GPIOC_OTYPER    0x00000000
#define VAL_GPIOC_OSPEEDR   0xFFFFFFFF
#define VAL_GPIOC_PUPDR     (PIN_PUDR_PULLUP(0) |                           \
                             PIN_PUDR_PULLUP(1) |                           \
                             PIN_PUDR_PULLUP(2) |                           \
                             PIN_PUDR_PULLUP(3) |                           \
                             PIN_PUDR_PULLUP(4) |                           \
                             PIN_PUDR_FLOATING(GPIOC_M1_DIAG) |             \
                             PIN_PUDR_PULLUP(GPIOC_UART_TX) |               \
                             PIN_PUDR_PULLUP(GPIOC_UART_RX) |               \
                             PIN_PUDR_FLOATING(GPIOC_M1_NSS) |              \
                             PIN_PUDR_FLOATING(GPIOC_MTR_A) |               \
                             PIN_PUDR_PULLUP(10) |                          \
                             PIN_PUDR_PULLUP(11) |                          \
                             PIN_PUDR_PULLUP(12) |                          \
                             PIN_PUDR_PULLUP(GPIOC_BUT1) |                  \
                             PIN_PUDR_PULLUP(14) |                          \
                             PIN_PUDR_PULLUP(15))
#define VAL_GPIOC_ODR       0xFFFFFFFF
#define VAL_GPIOC_AFRL      (PIN_AFIO_AF(GPIOC_UART_TX, 8) |                \
                             PIN_AFIO_AF(GPIOC_UART_RX, 8))
#define VAL_GPIOC_AFRH      0x00000000

/*
 * Port D setup.
 */
#define VAL_GPIOD_MODER     (PIN_MODE_INPUT(GPIOD_OSC_IN) |                 \
                             PIN_MODE_INPUT(GPIOD_OSC_OUT) |                \
                             PIN_MODE_INPUT(2) |                            \
                             PIN_MODE_INPUT(3) |                            \
                             PIN_MODE_INPUT(4) |                            \
                             PIN_MODE_INPUT(5) |                            \
                             PIN_MODE_INPUT(6) |                            \
                             PIN_MODE_INPUT(7) |                            \
                             PIN_MODE_INPUT(8) |                            \
                             PIN_MODE_INPUT(9) |                            \
                             PIN_MODE_INPUT(10) |                           \
                             PIN_MODE_INPUT(11) |                           \
                             PIN_MODE_INPUT(12) |                           \
                             PIN_MODE_INPUT(13) |                           \
                             PIN_MODE_INPUT(14) |                           \
                             PIN_MODE_INPUT(15))
#define VAL_GPIOD_OTYPER    0x00000000
#define VAL_GPIOD_OSPEEDR   0xFFFFFFFF
#define VAL_GPIOD_PUPDR     (PIN_PUDR_FLOATING(0) |                         \
                             PIN_PUDR_FLOATING(1) |                         \
                             PIN_PUDR_PULLUP(2) |                           \
                             PIN_PUDR_PULLUP(3) |                           \
                             PIN_PUDR_PULLUP(4) |                           \
                             PIN_PUDR_PULLUP(5) |                           \
                             PIN_PUDR_PULLUP(6) |                           \
                             PIN_PUDR_PULLUP(7) |                           \
                             PIN_PUDR_PULLUP(8) |                           \
                             PIN_PUDR_PULLUP(9) |                           \
                             PIN_PUDR_PULLUP(10) |                          \
                             PIN_PUDR_PULLUP(11) |                          \
                             PIN_PUDR_PULLUP(12) |                          \
                             PIN_PUDR_PULLUP(13) |                          \
                             PIN_PUDR_PULLUP(14) |                          \
                             PIN_PUDR_PULLUP(15))
#define VAL_GPIOD_ODR       0xFFFFFFFF
#define VAL_GPIOD_AFRL      0x00000000
#define VAL_GPIOD_AFRH      0x00000000

/*
 * Port E setup.
 */
#define VAL_GPIOE_MODER     0x00000000
#define VAL_GPIOE_OTYPER    0x00000000
#define VAL_GPIOE_OSPEEDR   0xFFFFFFFF
#define VAL_GPIOE_PUPDR     (PIN_PUDR_PULLUP(0) |                           \
                             PIN_PUDR_PULLUP(1) |                           \
                             PIN_PUDR_PULLUP(2) |                           \
                             PIN_PUDR_PULLUP(3) |                           \
                             PIN_PUDR_PULLUP(4) |                           \
                             PIN_PUDR_PULLUP(5) |                           \
                             PIN_PUDR_PULLUP(6) |                           \
                             PIN_PUDR_PULLUP(7) |                           \
                             PIN_PUDR_PULLUP(8) |                           \
                             PIN_PUDR_PULLUP(9) |                           \
                             PIN_PUDR_PULLUP(10) |                          \
                             PIN_PUDR_PULLUP(11) |                          \
                             PIN_PUDR_PULLUP(12) |                          \
                             PIN_PUDR_PULLUP(13) |                          \
                             PIN_PUDR_PULLUP(14) |                          \
                             PIN_PUDR_PULLUP(15))
#define VAL_GPIOE_ODR       0xFFFFFFFF
#define VAL_GPIOE_AFRL      0x00000000
#define VAL_GPIOE_AFRH      0x00000000

/*
 * Port F setup.
 */
#define VAL_GPIOF_MODER     0x00000000
#define VAL_GPIOF_OTYPER    0x00000000
#define VAL_GPIOF_OSPEEDR   0xFFFFFFFF
#define VAL_GPIOF_PUPDR     (PIN_PUDR_PULLUP(0) |                           \
                             PIN_PUDR_PULLUP(1) |                           \
                             PIN_PUDR_PULLUP(2) |                           \
                             PIN_PUDR_PULLUP(3) |                           \
                             PIN_PUDR_PULLUP(4) |                           \
                             PIN_PUDR_PULLUP(5) |                           \
                             PIN_PUDR_PULLUP(6) |                           \
                             PIN_PUDR_PULLUP(7) |                           \
                             PIN_PUDR_PULLUP(8) |                           \
                             PIN_PUDR_PULLUP(9) |                           \
                             PIN_PUDR_PULLUP(10) |                          \
                             PIN_PUDR_PULLUP(11) |                          \
                             PIN_PUDR_PULLUP(12) |                          \
                             PIN_PUDR_PULLUP(13) |                          \
                             PIN_PUDR_PULLUP(14) |                          \
                             PIN_PUDR_PULLUP(15))
#define VAL_GPIOF_ODR       0xFFFFFFFF
#define VAL_GPIOF_AFRL      0x00000000
#define VAL_GPIOF_AFRH      0x00000000

/*
 * Port G setup.
 */
#define VAL_GPIOG_MODER     0x00000000
#define VAL_GPIOG_OTYPER    0x00000000
#define VAL_GPIOG_OSPEEDR   0xFFFFFFFF
#define VAL_GPIOG_PUPDR     (PIN_PUDR_PULLUP(0) |                           \
                             PIN_PUDR_PULLUP(1) |                           \
                             PIN_PUDR_PULLUP(2) |                           \
                             PIN_PUDR_PULLUP(3) |                           \
                             PIN_PUDR_PULLUP(4) |                           \
                             PIN_PUDR_PULLUP(5) |                           \
                             PIN_PUDR_PULLUP(6) |                           \
                             PIN_PUDR_PULLUP(7) |                           \
                             PIN_PUDR_PULLUP(8) |                           \
                             PIN_PUDR_PULLUP(9) |                           \
                             PIN_PUDR_PULLUP(10) |                          \
                             PIN_PUDR_PULLUP(11) |                          \
                             PIN_PUDR_PULLUP(12) |                          \
                             PIN_PUDR_PULLUP(13) |                          \
                             PIN_PUDR_PULLUP(14) |                          \
                             PIN_PUDR_PULLUP(15))
#define VAL_GPIOG_ODR       0xFFFFFFFF
#define VAL_GPIOG_AFRL      0x00000000
#define VAL_GPIOG_AFRH      0x00000000

/*
 * Port H setup.
 */
#define VAL_GPIOH_MODER     0x00000000
#define VAL_GPIOH_OTYPER    0x00000000
#define VAL_GPIOH_OSPEEDR   0xFFFFFFFF
#define VAL_GPIOH_PUPDR     (PIN_PUDR_PULLUP(0) |                           \
                             PIN_PUDR_PULLUP(1) |                           \
                             PIN_PUDR_PULLUP(2) |                           \
                             PIN_PUDR_PULLUP(3) |                           \
                             PIN_PUDR_PULLUP(4) |                           \
                             PIN_PUDR_PULLUP(5) |                           \
                             PIN_PUDR_PULLUP(6) |                           \
                             PIN_PUDR_PULLUP(7) |                           \
                             PIN_PUDR_PULLUP(8) |                           \
                             PIN_PUDR_PULLUP(9) |                           \
                             PIN_PUDR_PULLUP(10) |                          \
                             PIN_PUDR_PULLUP(11) |                          \
                             PIN_PUDR_PULLUP(12) |                          \
                             PIN_PUDR_PULLUP(13) |                          \
                             PIN_PUDR_PULLUP(14) |                          \
                             PIN_PUDR_PULLUP(15))
#define VAL_GPIOH_ODR       0xFFFFFFFF
#define VAL_GPIOH_AFRL      0x00000000
#define VAL_GPIOH_AFRH      0x00000000

/*
 * Port I setup.
 */
#define VAL_GPIOI_MODER     0x00000000
#define VAL_GPIOI_OTYPER    0x00000000
#define VAL_GPIOI_OSPEEDR   0xFFFFFFFF
#define VAL_GPIOI_PUPDR     (PIN_PUDR_PULLUP(0) |                           \
                             PIN_PUDR_PULLUP(1) |                           \
                             PIN_PUDR_PULLUP(2) |                           \
                             PIN_PUDR_PULLUP(3) |                           \
                             PIN_PUDR_PULLUP(4) |                           \
                             PIN_PUDR_PULLUP(5) |                           \
                             PIN_PUDR_PULLUP(6) |                           \
                             PIN_PUDR_PULLUP(7) |                           \
                             PIN_PUDR_PULLUP(8) |                           \
                             PIN_PUDR_PULLUP(9) |                           \
                             PIN_PUDR_PULLUP(10) |                          \
                             PIN_PUDR_PULLUP(11) |                          \
                             PIN_PUDR_PULLUP(12) |                          \
                             PIN_PUDR_PULLUP(13) |                          \
                             PIN_PUDR_PULLUP(14) |                          \
                             PIN_PUDR_PULLUP(15))
#define VAL_GPIOI_ODR       0xFFFFFFFF
#define VAL_GPIOI_AFRL      0x00000000
#define VAL_GPIOI_AFRH      0x00000000

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
