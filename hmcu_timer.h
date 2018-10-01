/**
 * @file hmcu_timer.h
 *
 * @brief Interface for hardware layer for software timer.
 *
 * This project is licensed under the MIT License.
 *
 * Copyright (c) 2018, Lincoln Uehara
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SRC_HMCU_TIMER_H_
#define SRC_HMCU_TIMER_H_

/*****************************************************************************
 * Public constants.
 *****************************************************************************/

/**
 * @brief Maximum number of simultaneously allocated software timer instances.
 */
#define SOFT_TIMER_MAX_INSTANCES 10

/**
 * @brief Maximum timeout value in milliseconds for a software timer.
 */
#define SOFT_TIMER_MAX_RELOAD_MS 100000000

/*****************************************************************************
 * Public types.
 *****************************************************************************/
/**
 * @brief
 */
typedef struct soft_timer{
	uint32_t value;
}soft_timer;

/*****************************************************************************
 * Public functions.
 *****************************************************************************/
extern void _hmcu_init(void);
extern void _hmcu_enableIRQ(void);
extern void _hmcu_disableIRQ(void);
extern void _hmcu_startTimer(void);
extern void _hmcu_stopTimer(void);
extern void _hmcu_setPrescaler(uint16_t prescalerFlag);
extern uint16_t _hmcu_readPrescaler(void);
extern uint16_t _hmcu_readCountdown(void);
extern void _hmcu_setCountdown(uint16_t cdValue);

#endif /* SRC_HMCU_TIMER_H_ */
