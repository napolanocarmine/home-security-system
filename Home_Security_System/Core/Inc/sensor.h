/*
 * sensor.h
 *
 *  Created on: May 24, 2020
 *      Author: emiso
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

/**
 * Struct to define the sensor state
 */
typedef enum{
	SENSOR_ACTIVE,
	SENSOR_INACTIVE,
	SENSOR_ALARMED,
	SENSOR_DELAYED
}module_state_t;

/**
 * Struct to define the state, pin, port of a sensor
 */
typedef struct{

	GPIO_TypeDef *GPIOx;

	uint16_t GPIO_Pin;

	GPIO_PinState pin_state;

}digital_sensor_t;


#endif /* INC_SENSOR_H_ */
