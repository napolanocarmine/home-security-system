/*
 * handle_timer.h
 *
 *  Created on: May 24, 2020
 *      Author: 2017
 */

#ifndef INC_TIMER_HANDLER_H_
#define INC_TIMER_HANDLER_H_

#include <configuration_protocol.h>
#include <stdint.h>
#include <timer_handler.h>
#include "stm32f4xx_hal.h"



/**
 * Initialize timer with given prescaler and period
 */
void init_timer(TIM_HandleTypeDef *timer, uint32_t prescaler, uint32_t period);


/**
 * Set timer's period.
 */
void set_timer_period(TIM_HandleTypeDef *timer, uint32_t period);

/**
 * Start timer in interrupt mode
 */
void start_timer_IT(TIM_HandleTypeDef *timer);

/**
 * Stop timer in interrupt mode
 */
void stop_timer_IT(TIM_HandleTypeDef *timer);

/**
 * Reset the counter of the given timer
 */
void reset_timer_counter(TIM_HandleTypeDef *timer);

#endif /* INC_TIMER_HANDLER_H_ */
