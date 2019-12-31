/**
 * @file hmcu_timer.h
 *
 * @brief Interface for hardware layer for software timer.
 *
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
