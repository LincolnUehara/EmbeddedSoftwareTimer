/**
 * @file soft_timer.c
 *
 * @brief Implementation of Software Timer.
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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "soft_timer.h"
#include "hmcu_timer.h"

/*****************************************************************************
 * Private types.
 *****************************************************************************/
typedef struct tmr_instance{
	soft_timer_t            * p_timer;
	soft_timer_callback_t	timeout_cb;
	uint32_t                reload_ms;
	bool                    repeat;
	bool					isSet;
	bool					inUse;
	uint32_t                countdown;
	struct tmr_instance 	* prev;
	struct tmr_instance 	* next;
}tmr_instance;

typedef struct tmr_instance_track{
	struct tmr_instance		* first;
	struct tmr_instance		* last;
}tmr_instance_track;

/*****************************************************************************
 * Global variables.
 *****************************************************************************/
static tmr_instance_track	list_head_tail;
static uint8_t				list_items_qty = 0;
static tmr_instance			* queue_sorted[SOFT_TIMER_MAX_INSTANCES];
static uint8_t				queue_items_qty = 0;
static uint16_t				last_updated_value = 0;
static bool					soft_timer_initialized = false;
static bool					soft_timer_irq_handled = false;

/*****************************************************************************
 * Prototypes for private functions.
 *****************************************************************************/
/* Prototypes related to software time instances list. */
static void 		_st_LIST_createInstance(soft_timer_t * p_timer);
static void 		_st_LIST_destroyInstance(soft_timer_t * p_timer);
static tmr_instance * _st_LIST_whereInstance(soft_timer_t * p_timer);

/* Prototypes related to software time instances queue. */
static void 		_st_QUEUE_addInstance(tmr_instance * tmr_inst);
static void 		_st_QUEUE_removeInstance(tmr_instance * tmr_inst);
static void			_st_QUEUE_updateCountdown(void);
static void			_st_QUEUE_juxtaposeItems(void);
static void			_st_QUEUE_sortByCountdown(void);
static void			_st_QUEUE_parserAndSet(void);

/*****************************************************************************
 * Bodies of public functions.
 *****************************************************************************/

void soft_timer_init(void){

	uint8_t i;

	/* Initialize global variables. */
	for(i = 0 ; i < SOFT_TIMER_MAX_INSTANCES; i++){
		queue_sorted[i] = NULL;
	}
	list_head_tail.first = NULL;
	list_head_tail.last = NULL;
	soft_timer_initialized = true;

	/* Initialize hardware timer. */
	_hmcu_init();
	_hmcu_setCountdown(0);
	_hmcu_setPrescaler(1);
	_hmcu_stopTimer();
	_hmcu_disableIRQ();
}

void soft_timer_create(soft_timer_t *p_timer){

	tmr_instance * tmp_ptr;

	/* If soft_timer_init() function was not called yet, just return.*/
	if(!soft_timer_initialized){
		return;
	}

	/* If the double pointer is addressing to NULL, just return. */
	if(p_timer == NULL){
		return;
	}

	/* If the number of already existing instances surpasses the limit,
	 * just return. */
	if(list_items_qty >= SOFT_TIMER_MAX_INSTANCES){
		return;
	}

	/* Obtain the address of respective timer instance, if it already
	 * exists. If not yet (returns NULL), go to the conditional to
	 * create a new instance. */
	tmp_ptr = _st_LIST_whereInstance(p_timer);

	if(tmp_ptr == NULL){

		_hmcu_disableIRQ();
		_st_LIST_createInstance(p_timer);
		_hmcu_enableIRQ();
	}
}

soft_timer_status_t soft_timer_set(soft_timer_t          *p_timer,
                                   soft_timer_callback_t  timeout_cb,
                                   uint32_t               reload_ms,
                                   bool                   repeat){

	tmr_instance * tmp_ptr;

	/* If soft_timer_init() function was not called yet, just return.*/
	if(!soft_timer_initialized){
		return SOFT_TIMER_STATUS_INVALID_STATE;
	}

	/* Obtain the address of respective timer instance. If it was not
	 * created yet, return invalid parameter. */
	if(p_timer == NULL){
		return SOFT_TIMER_STATUS_INVALID_PARAMETER;
	}else{
		tmp_ptr = _st_LIST_whereInstance(p_timer);
	}if(tmp_ptr == NULL){
		return SOFT_TIMER_STATUS_INVALID_PARAMETER;
	}

	/* Return respective flag to its respective condition. */
	if(timeout_cb == NULL){
		return SOFT_TIMER_STATUS_INVALID_PARAMETER;
	}
	if(reload_ms > 1000000){
		return SOFT_TIMER_STATUS_INVALID_PARAMETER;
	}

	/* Attribute respective parameters. */
	tmp_ptr->p_timer	= p_timer;
	tmp_ptr->timeout_cb = timeout_cb;
	tmp_ptr->reload_ms	= reload_ms;
	tmp_ptr->repeat		= repeat;
	tmp_ptr->isSet		= true;
	tmp_ptr->inUse		= false;
	tmp_ptr->countdown	= reload_ms;

	return SOFT_TIMER_STATUS_SUCCESS;
}

soft_timer_status_t soft_timer_start(soft_timer_t *p_timer){

	tmr_instance * tmp_ptr;

	/* If soft_timer_init() function was not called yet, just return.*/
	if(!soft_timer_initialized){
		return SOFT_TIMER_STATUS_INVALID_STATE;
	}

	/* Obtain the address of respective timer instance. If it was not
	 * created yet, return invalid parameter. */
	if(p_timer == NULL){
		return SOFT_TIMER_STATUS_INVALID_PARAMETER;
	}else{
		tmp_ptr = _st_LIST_whereInstance(p_timer);
	}if(tmp_ptr == NULL){
		return SOFT_TIMER_STATUS_INVALID_PARAMETER;
	}

	/* If the instance was not set yet, return invalid state. */
	if(!tmp_ptr->isSet){
    	return SOFT_TIMER_STATUS_INVALID_STATE;
    }

	/* If all the conditions are OK, disable IRQ and stop hardware timer,
	 * check if the item is already in use on the queue. If yes, return
	 * invalid state. */
	_hmcu_disableIRQ();
	_hmcu_stopTimer();
	if(tmp_ptr->inUse){
		if(!soft_timer_irq_handled){
			_hmcu_startTimer();
			_hmcu_enableIRQ();
		}
		return SOFT_TIMER_STATUS_INVALID_STATE;
	}

	/* Add the item in the queue of execution. */
	_st_QUEUE_addInstance(tmp_ptr);

	/* If the IRQ is not handled, start hardware timer and enable IRQ again. */
	if(!soft_timer_irq_handled){
		_hmcu_startTimer();
		_hmcu_enableIRQ();
	}

	return SOFT_TIMER_STATUS_SUCCESS;
}

soft_timer_status_t soft_timer_stop(soft_timer_t *p_timer){

	tmr_instance * tmp_ptr;

	/* If soft_timer_init() function was not called yet, just return.*/
	if(!soft_timer_initialized){
		return SOFT_TIMER_STATUS_INVALID_STATE;
	}

	/* Obtain the address of respective timer instance. If it was not
	 * created yet, return invalid parameter. */
	if(p_timer == NULL){
		return SOFT_TIMER_STATUS_INVALID_PARAMETER;
	}else{
		tmp_ptr = _st_LIST_whereInstance(p_timer);
	}if(tmp_ptr == NULL){
		return SOFT_TIMER_STATUS_INVALID_PARAMETER;
	}

	/* If the instance was not set yet, return invalid state. */
	if(!tmp_ptr->isSet){
		return SOFT_TIMER_STATUS_INVALID_STATE;
	}

	/* If all the conditions are OK, disable IRQ and stop hardware timer,
	 * check if the item is not in use on the queue. If not, return
	 * invalid state. */
	_hmcu_disableIRQ();
	_hmcu_stopTimer();
	if(!tmp_ptr->inUse){
		if(!soft_timer_irq_handled){
			_hmcu_startTimer();
			_hmcu_enableIRQ();
		}
		return SOFT_TIMER_STATUS_INVALID_STATE;
	}

	/* Remove the item from the queue of execution. */
	_st_QUEUE_removeInstance(tmp_ptr);

	/* If the IRQ is not handled, start hardware timer and enable IRQ again. */
	if(!soft_timer_irq_handled){
		_hmcu_startTimer();
		_hmcu_enableIRQ();
	}

	return SOFT_TIMER_STATUS_SUCCESS;
}

void soft_timer_destroy(soft_timer_t *p_timer){
	
	tmr_instance * tmp_ptr;

	/* If soft_timer_init() function was not called yet, just return.*/
	if(!soft_timer_initialized){
		return;
	}

	/* If the double pointer is addressing to NULL, just return. */
	if(p_timer == NULL){
		return;
	}

	/* Obtain the address of respective timer instance. If it exists
	 * and is not in use (is not on queue of execution), run the
	 * conditional to destroy it. */
	tmp_ptr = _st_LIST_whereInstance(p_timer);

	if((tmp_ptr != NULL) && (!tmp_ptr->inUse)){

		_hmcu_disableIRQ();
		_st_LIST_destroyInstance(p_timer);
		_hmcu_enableIRQ();
	}
}

void soft_timer_irq_handler(void){

	/* Atribute true to IRQ handled and disable it and stop the hardware
	 * timer. */
	soft_timer_irq_handled = true;
	_hmcu_disableIRQ();
	_hmcu_stopTimer();

	/* Update the countdown of all items on the queue, and execute the
	 * callback function. */
	_st_QUEUE_updateCountdown();
	queue_sorted[0]->timeout_cb(queue_sorted[0]->p_timer);

	/* Check if the first item's countdown reached to zero. If yes, but
	 * is set to repeat, reload the value and sort the queue. If yes
	 * but is not set to repeat, remove it from the queue and juxtapose
	 * the items. If there is no more items on the queue, just return. */
	if(queue_sorted[0]->countdown == 0){

		if(queue_sorted[0]->repeat){

			queue_sorted[0]->countdown = queue_sorted[0]->reload_ms;
			_st_QUEUE_sortByCountdown();

		}else{

			_st_QUEUE_removeInstance(queue_sorted[0]);

			if(queue_items_qty == 0){

				soft_timer_irq_handled = false;
				return;

			}else{

				_st_QUEUE_juxtaposeItems();

			}
		}
	}

	/* If there is item on queue, set the registers and start it. */
	_st_QUEUE_parserAndSet();
	_hmcu_startTimer();
	_hmcu_enableIRQ();
	soft_timer_irq_handled = false;
}

/*****************************************************************************
 * Bodies of private functions.
 *****************************************************************************/

static void _st_LIST_createInstance(soft_timer_t * p_timer){

	/* Create a temporary variable and allocate its location by size of
	 * tmr_instance struct. */
	tmr_instance * tmp_ptr;
	tmp_ptr = malloc(sizeof(tmr_instance));

	/* Set some parameters of registering instance. Add by one the
	 * number of existing instances. */
	tmp_ptr->p_timer = p_timer;
	tmp_ptr->isSet = false;
	list_items_qty++;

	/* The next pointer in the linked list is a NULL value to be
	 * aware that it is the last item. */
	tmp_ptr->next = NULL;

	/* If it is the first item, attribute NULL to previous pointer and
	 * set the list_head_tail's pointer. If it is not, set the included
	 * item's previous pointer to the next-to-last item, and its next
	 * pointer. */
	if(list_items_qty == 1){
		tmp_ptr->prev = NULL;
		list_head_tail.first = tmp_ptr;
	}else{
		tmp_ptr->prev = list_head_tail.last;
		list_head_tail.last->next = tmp_ptr;
	}

	/* Set the list_head_tail pointer to the included item. */
	list_head_tail.last = tmp_ptr;
}

static void _st_LIST_destroyInstance(soft_timer_t * p_timer){
	
	/* Create a temporary pointer and run through the items, to find p_timer
	 * instance. If reached to last item without finding it, just exit. */
	tmr_instance *tmp_ptr;
	tmp_ptr = list_head_tail.first;

	while(tmp_ptr->p_timer != p_timer){
		if(tmp_ptr->next == NULL){
			return;
		}
		tmp_ptr = tmp_ptr->next;
	}

	/* If found the instance but it is the only item, just set the
	 * pointers of list_head_tail. */
	if((tmp_ptr->prev == NULL) && (tmp_ptr->next == NULL)){
		list_head_tail.first = NULL;
		list_head_tail.last = NULL;
	}

	/* If found the instance but it is the first item, set the
	 * list_head_tail pointer and the next item's pointer. */
	else if((tmp_ptr->prev == NULL) && (tmp_ptr->next != NULL)){
		list_head_tail.first = tmp_ptr->next;
		tmp_ptr->next->prev = NULL;
	}

	/* If found the instance but it is the last item, set the
	 * list_head_tail pointer and the previous item's pointer. */
	else if((tmp_ptr->prev != NULL) && (tmp_ptr->next == NULL)){
		list_head_tail.last = tmp_ptr->prev;
		tmp_ptr->prev->next = NULL;
	}

	/* If found the instance in the middle of list, set the both next and
	 * previous item pointers. */
	else{
		tmp_ptr->prev->next = tmp_ptr->next;
		tmp_ptr->next->prev = tmp_ptr->prev;
	}

	/* Finally, deallocate the referent address and decrement the number
	 * of existing items. */
	free(tmp_ptr);
	list_items_qty--;
}

static tmr_instance * _st_LIST_whereInstance(soft_timer_t * p_timer){

	/* Create a temporary pointer and run through the items, to find
	 * p_timer instance. If reached to last item without finding it,
	 * or there is no items on the list, return NULL. This pointer
	 * is static to avoid segmentation fault problems.*/
	static tmr_instance * tmp_ptr;
	tmp_ptr = list_head_tail.first;

	while(tmp_ptr->p_timer != p_timer){
		if((tmp_ptr->next == NULL)||(tmp_ptr == NULL)){
			return NULL;
		}
		tmp_ptr = tmp_ptr->next;
	}

	/* If found, return it's address. */
	return tmp_ptr;
}

static void _st_QUEUE_addInstance(tmr_instance * tmr_inst){

	/* Update the countdown values of every item in the queue. */
	if(!soft_timer_irq_handled){
		_st_QUEUE_updateCountdown();
	}

	/* Attribute the address, set that it is used at queue, and update the
	 * countdown value, if it was used before. */
	queue_sorted[queue_items_qty] = tmr_inst;
	queue_sorted[queue_items_qty]->inUse = true;
	queue_sorted[queue_items_qty]->countdown =
				queue_sorted[queue_items_qty]->reload_ms;

	/* Increment the number of existing items at the queue. */
	queue_items_qty++;

	/* Organize the queue order, set the registers and start the hardware
	 * timer. */
	_st_QUEUE_sortByCountdown();
	_st_QUEUE_parserAndSet();
}

static void _st_QUEUE_removeInstance(tmr_instance *tmr_inst){

	uint8_t index = 0;

	/* Find the index of the referent timer instance in the queue. */
	while(queue_sorted[index] != tmr_inst){
		index++;
	}

	/* Attribute false to indicate that it is no more at the queue,
	 * and attribute its address to NULL. */
	queue_sorted[index]->inUse = false;
	queue_sorted[index] = NULL;

	/* Decrement the number of items on the queue and juxtapose its items. */
	queue_items_qty--;
	_st_QUEUE_juxtaposeItems();

	/* If it was the first element to be deleted, so adjust the
	 * countdown values and set the registers. */
	if(index == 0){
		_st_QUEUE_updateCountdown();
		_st_QUEUE_parserAndSet();
	}
}

static void	_st_QUEUE_updateCountdown(void){

	uint8_t i;
	uint16_t cdValue, prescalerConstant, prescalerFlag;

	/* Check the current prescaler to atribute a value to a constant to
	 * be multiplied to countdown value.  */
	prescalerFlag = _hmcu_readPrescaler();

	if(prescalerFlag == 1){
		prescalerConstant = 1;
	}else if(prescalerFlag == 10){
		prescalerConstant = 10;
	}else if(prescalerFlag == 100){
		prescalerConstant = 100;
	}

	/* Update countdown variable of every item. */
	cdValue = _hmcu_readCountdown()*prescalerConstant;
	for(i = 0 ; i < queue_items_qty ; i++){
		queue_sorted[i]->countdown -= cdValue;
	}
}

static void	_st_QUEUE_juxtaposeItems(void){

	uint8_t index = 0;

	/* Juxtapose the items inside the queue. */
	while(index < queue_items_qty){
		if(queue_sorted[index] == NULL){
			queue_sorted[index] = queue_sorted[index+1];
			queue_sorted[index+1] = NULL;
		}
		index++;
	}
}

static void	_st_QUEUE_sortByCountdown(void){

	tmr_instance * tmp_ptr;
	uint8_t i, j;

	/* Bubble sort to reorder by ascending order, according to countdown value. */
	for (i = 0; i < queue_items_qty-1; i++){
		/* Last i elements are already in place */
		for (j = 0; j < queue_items_qty-i-1; j++){
			if (queue_sorted[j]->countdown > queue_sorted[j+1]->countdown){
				tmp_ptr = queue_sorted[j+1];
				queue_sorted[j+1] = queue_sorted[j];
				queue_sorted[j] = tmp_ptr;
			}
		}
	}
}

static void	_st_QUEUE_parserAndSet(void){

	uint16_t cntValue_1, cntValue_10, cntValue_100;

	/* 'Parse' the countdown value and set the prescaler and CNT
	 * register.
	 * Since CNT register is 16 bits, we can store up to number 65535.
	 * So, the idea here is to store in this register a number up to
	 * 10000, and for numbers above its needed to set prescaler, and
	 * divide the number to be store to its referent prescaler.
	 * In this way, times up to 9.999 ms will have a maximum delay of
	 * 1 ms, up to 99.999 ms will have maximum delay of 10 ms, and so
	 * on.
	 * For example, parsing 28.543 ms to countdown: first, the number
	 * 8543 will be write to CNT register and set 1 as prescaler on
	 * CTRL register, having 1 ms of imprecision. On the other moment,
	 * 2000 will be write on CNT register and set 10 as prescaler on
	 * CTRL register, having 10 ms of imprecision. */

	cntValue_1 = (uint16_t)((queue_sorted[0]->countdown)%10000);
	if(cntValue_1 != 0){
		_hmcu_setPrescaler(1);
		_hmcu_setCountdown(cntValue_1);
		last_updated_value = cntValue_1;
		return;
	}

	cntValue_10 = (uint16_t)((queue_sorted[0]->countdown)%100000);
	if(cntValue_10 != 0){
		_hmcu_setPrescaler(10);
		_hmcu_setCountdown((cntValue_10)/10);
		last_updated_value = cntValue_10;
		return;
	}

	cntValue_100 = (uint16_t)((queue_sorted[0]->countdown)%1000000);
	if(cntValue_100 != 0){
		_hmcu_setPrescaler(100);
		_hmcu_setCountdown((cntValue_100)/100);
		last_updated_value = cntValue_100;
		return;
	}
}
