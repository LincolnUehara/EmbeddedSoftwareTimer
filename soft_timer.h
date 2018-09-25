/**
 * @file soft_timer.h
 *
 * @brief Interface for Software Timer.
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

#ifndef __SOFT_TIMER_H__
#define __SOFT_TIMER_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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
 * @brief Forward declaration of software timer instance.
 */
typedef struct soft_timer soft_timer_t;

/**
 * @brief
 */
typedef struct soft_timer{
	uint16_t value;
}soft_timer;

/**
 * @brief Type for timeout callbacks.
 *
 * @param p_timer Pointer to timer triggering timeout callback.
 */
typedef void (*soft_timer_callback_t)(soft_timer_t *p_timer);

/**
 * @brief Status codes for software timer functions.
 */
typedef enum soft_timer_status
{
    SOFT_TIMER_STATUS_SUCCESS = 0,       /**< Operation succeeded. */
    SOFT_TIMER_STATUS_INVALID_PARAMETER, /**< Failure: invalid parameter. */
    SOFT_TIMER_STATUS_INVALID_STATE     /**< Failure: invalid timer state. */
} soft_timer_status_t;

/*****************************************************************************
 * Public functions.
 *****************************************************************************/
void hmcu_timer_irq_handler(void);
/**
 * @brief Initialize software timer module.
 */
void soft_timer_init(void);

/**
 * @brief Allocate and initialize software timer instance.
 *
 * @param pp_timer Output parameter: Pointer to newly created software timer
 *                 instance.
 */
void soft_timer_create(soft_timer_t *p_timer);

/**
 * @brief Configure countdown timer.
 *
 * @param p_timer    Pointer to timer instance to be configured.
 * @param timeout_cb Pointer to timeout callback function.
 * @param reload_ms  Value to reload timer in milliseconds.
 * @param repeat     Boolean flag signalling if timer should repeat after
 *                   timeout.
 *
 * @return Operation status. Check @ref soft_timer_status_t.
 */
soft_timer_status_t soft_timer_set(soft_timer_t          *p_timer,
                                   soft_timer_callback_t  timeout_cb,
                                   uint32_t               reload_ms,
                                   bool                   repeat);

/**
 * @brief Start timer.
 *
 * @param p_timer    Pointer to timer instance to be started.
 *
 * @return Operation status. Check @ref soft_timer_status_t.
 */
soft_timer_status_t soft_timer_start(soft_timer_t *p_timer);

/**
 * @brief Stop timer.
 *
 * @param p_timer    Pointer to timer instance to be started.
 *
 * @return Operation status. Check @ref soft_timer_status_t.
 */
soft_timer_status_t soft_timer_stop(soft_timer_t *p_timer);

/**
 * @brief Deallocate software timer instance.
 *
 * @param pp_timer Input / Output parameter: Pointer to destroyed software
 *                 timer instance.
 */
void soft_timer_destroy(soft_timer_t *p_timer);

#endif /** __SOFT_TIMER_H__ */
