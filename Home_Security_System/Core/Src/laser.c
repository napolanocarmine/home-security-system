/*
 * laser.c
 *
 *  Created on: Jun 1, 2020
 *      Author: 2017
 */

#include "laser.h"
#include "stm32f4xx.h"

/**
 * @brief  Initialize the laser
 * @param  laser        pointer to laser structure
 * @param  GPIOx        pointer to GPIO port
 * @param  GPIO_Pin     the GPIO_Pin associated to laser
 * @param  pin_state    the state of the GPIO_Pin
 */
void init_laser(laser_t *laser, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState pin_state){

	laser->GPIOx = GPIOx;

	laser->GPIO_Pin = GPIO_Pin;

	laser->pin_state = pin_state;

	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, pin_state);

}

/**
 * @brief  Set the laser pin
 * @param  laser   pointer to laser structure
 */
void set_laser(laser_t *laser){

	HAL_GPIO_WritePin(laser->GPIOx, laser->GPIO_Pin, GPIO_PIN_SET);
	laser->pin_state = GPIO_PIN_SET;

}

/**
 * @brief  Reset the laser pin
 * @param  laser   pointer to laser structure
 */
void reset_laser(laser_t *laser){

	HAL_GPIO_WritePin(laser->GPIOx, laser->GPIO_Pin, GPIO_PIN_RESET);
	laser->pin_state = GPIO_PIN_RESET;

}

