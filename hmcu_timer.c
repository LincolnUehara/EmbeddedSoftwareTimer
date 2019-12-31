/**
 * @file hmcu_timer.c
 *
 * @brief Implementation of hardware layer for software timer.
 *
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*****************************************************************************
 * Libraries needed by hardware MCU
 *****************************************************************************/
#include <inc/hw_memmap.h>
#include <inc/tm4c123gh6pm.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/interrupt.h>

/*****************************************************************************
 * Bodies of public functions used in soft_timer.c
 *****************************************************************************/
void _hmcu_init(void){

	/* Initialize all it is needed for hardware MCU.
	 * Do not make it repeatable, and in increment order of countdown. */
}

void _hmcu_enableIRQ(void){

	/* Clear and enable the interrupts related to the timer. */
}

void _hmcu_disableIRQ(void){

	/* Disable the interrupts related to the timer, and clear the interrupts. */
}

void _hmcu_startTimer(void){

	/* Only start the hardware timer. */
}

void _hmcu_stopTimer(void){

	/* Only stop the hardware timer. */
}

void _hmcu_setPrescaler(uint16_t prescalerFlag){

	/* Set the prescaler of hardware timer MCU. */
}

uint16_t _hmcu_readPrescaler(void){

	/* Read the prescaler of hardware timer MCU. */

	uint32_t prcValue = 0;

	/* prcValue = TimerPrescaleGet(); */

	return prcValue;
}

uint16_t _hmcu_readCountdown(void){

	/* Read the actual countdown value from the register.
	 * Beware the clock frequency of your MCU, and convert the value to
	 * milliseconds. */

	uint32_t cdValue = 0;

	/* cdValue = TimerValueGet()/(ClockGet()/1000); */

	return cdValue;
}

void _hmcu_setCountdown(uint16_t cdValue){

	/* Set the countdown value at the register.
	 * Beware the clock frequency of your MCU, and convert the value. */

	uint32_t loadValue = 0;

	/* loadValue = (ClockGet()/1000)*cdValue; */

	TimerLoadSet(loadValue);
}
