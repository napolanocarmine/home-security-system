/*
 * sensor_pir.h
 *
 *  Created on: May 24, 2020
 *      Author: emiso
 */

#ifndef INC_MODULE_PIR_H_
#define INC_MODULE_PIR_H_

#include "sensor.h"
#include "stm32f4xx_hal.h"

/*
 * Define pir struct
 */
typedef struct{

	digital_sensor_t *sensor;
	module_state_t state;
	TIM_HandleTypeDef *timer;
	uint16_t pulse;
	uint8_t delay;

}module_pir_t;


/**
 * To initialize the pir sensor
 */
void init_pir(module_pir_t *pir, digital_sensor_t*sensor_pir,GPIO_PinState pin_state, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, module_state_t state, uint8_t delay, TIM_HandleTypeDef *timer, uint16_t pulse);

/**
 * To get the state of sensor pir
 */
module_state_t get_state_pir(module_pir_t *pir);

/**
 * To set the state of sensor pir
 */
void set_state_pir(module_pir_t *pir, module_state_t state);

/**
 * Get the value of the pir's GPIO Pin
 */
GPIO_PinState get_pir_pin_state(module_pir_t *pir);

/**
 * Set the pir's GPIO Pin state
 */
void set_pir_pin_state(module_pir_t *pir, GPIO_PinState state);

#endif /* INC_MODULE_PIR_H_ */
