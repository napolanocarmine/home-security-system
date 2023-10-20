/*
 * system_led.c
 *
 *  Created on: May 31, 2020
 *      Author: 2017
 */

#include "system_led.h"

/**
 * @brief  Turn on the system led
 */
void turn_on_system_led(){

	HAL_GPIO_WritePin(SYSTEM_LED_PORT, SYSTEM_LED_PIN, GPIO_PIN_SET);

}

/**
 * @brief  Turn off the system led
 */
void turn_off_system_led(){

	HAL_GPIO_WritePin(SYSTEM_LED_PORT, SYSTEM_LED_PIN, GPIO_PIN_RESET);

}

/**
 * @brief  Toggle the system led state
 */
void toggle_system_led(){

	HAL_GPIO_TogglePin(SYSTEM_LED_PORT, SYSTEM_LED_PIN);

}
