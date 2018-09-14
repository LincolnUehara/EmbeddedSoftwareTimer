/**
 * @file soft_timer.c
 *
 * @brief Implementation of Software Timer.
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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*****************************************************************************
 * Private constants.
 *****************************************************************************/


/*****************************************************************************
 * Private macros.
 *****************************************************************************/


/*****************************************************************************
 * Private types.
 *****************************************************************************/


/*****************************************************************************
 * Global variables.
 *****************************************************************************/


/*****************************************************************************
 * Prototypes for private functions.
 *****************************************************************************/
/* Prototypes related to TIMER_CTRL Register. */
static void 		_st_CTRLREG_enableIRQ(void);
static void 		_st_CTRLREG_disableIRQ(void);
static void 		_st_CTRLREG_startHWTimer(void);
static void 		_st_CTRLREG_stopHWTimer(void);
static void 		_st_CTRLREG_repeatDisable(void);
static void 		_st_CTRLREG_setPrescaler(uint16_t prescalerFlag);
static uint16_t		_st_CTRLREG_readPrescaler(void);

/* Prototypes related to TIMER_CNT Register. */
static uint16_t		_st_CNTREG_currentCountdown(void);
static void 		_st_CNTREG_setCountdown(uint16_t cdValue);

/* Prototypes related to TIMER_RLD Register. */
static void 		_st_RLDREG_setReloadValue(uint16_t cdValue);

/*****************************************************************************
 * Bodies of private functions.
 *****************************************************************************/
static void _st_CTRLREG_enableIRQ(void){


}

static void _st_CTRLREG_disableIRQ(void){


}

static void _st_CTRLREG_startHWTimer(void){


}

static void _st_CTRLREG_stopHWTimer(void){


}

static void _st_CTRLREG_repeatDisable(void){


}

static void	_st_CTRLREG_setPrescaler(uint16_t prescalerFlag){


}

static uint16_t	_st_CTRLREG_readPrescaler(void){


}

static uint16_t _st_CNTREG_currentCountdown(void){


}

static void _st_CNTREG_setCountdown(uint16_t cdValue){


}

static void _st_RLDREG_setReloadValue(uint16_t cdValue){


}
