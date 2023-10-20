/*
 * laser.h
 *
 *  Created on: 1 Jun 2020
 *      Author: 2017
 */

#ifndef INC_LASER_H_
#define INC_LASER_H_

#include "stm32f4xx.h"

/**
 * Define laser struct
 */
typedef struct{

	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
	GPIO_PinState pin_state;

}laser_t;

/**
 * Initialize the laser
 */
void init_laser(laser_t *laser, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState pin_state);

/**
 * Set a status to laser and its pin
 */
void set_laser(laser_t *laser);

/**
 * Reset the status to laser and its pin
 */
void reset_laser(laser_t *laser);

#endif /* INC_LASER_H_ */
