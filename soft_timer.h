/**
 * @file soft_timer.h
 *
 * @brief Interface for Software Timer.
 *
 * Copyright (c) 2018, Lincoln Uehara
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
void soft_timer_create(soft_timer_t **pp_timer);

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
void soft_timer_destroy(soft_timer_t **pp_timer);

#endif /** __SOFT_TIMER_H__ */
