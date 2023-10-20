/*
 * system_led.h
 *
 *  Created on: May 31, 2020
 *      Author: 2017
 */

#ifndef INC_SYSTEM_LED_H_
#define INC_SYSTEM_LED_H_

#include "stm32f4xx.h"

#define SYSTEM_LED_PORT GPIOA
#define SYSTEM_LED_PIN GPIO_PIN_5

/**
 * Turn on the system led
 */
void turn_on_system_led();

/**
 * Turn off the system led
 */
void turn_off_system_led();

/**
 * Toggle the system led state
 */
void toggle_system_led();

#endif /* INC_SYSTEM_LED_H_ */
