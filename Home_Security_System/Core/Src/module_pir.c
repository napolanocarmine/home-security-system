/*
 * sensor_pir.c
 *
 *  Created on: May 24, 2020
 *      Author: emiso
 */

#include "module_pir.h"

/**
 * @brief  Initialize the pir sensor
 * @param  pir			pointer to module pir structure
 * @param  sensor_pir	pointer to digital sensor structure
 * @param  pin_state	state of GPIO_Pin
 * @param  GPIOx		pointer to GPIO port
 * @param  GPIO_Pin		the GPIO_Pin associated to pir module
 * @param  state		pir module state
 * @param  delay		alarm delay value
 * @param  timer		the associated timer for counting the time for signal stability
 * @param  pulse		pulse value for the associated ringtone
 */
void init_pir(module_pir_t *pir, digital_sensor_t *sensor_pir,GPIO_PinState pin_state, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, module_state_t state, uint8_t delay, TIM_HandleTypeDef *timer, uint16_t pulse){


	sensor_pir->GPIO_Pin = GPIO_Pin;
	sensor_pir->GPIOx = GPIOx;
	sensor_pir->pin_state = pin_state;
	pir->sensor = sensor_pir;
	pir->timer = timer;
	pir->pulse = pulse;
	pir->state = state;
	pir->delay = delay;
}

/**
 * @brief   Get the state of sensor pir
 * @param   pir 	pointer to module pir structure
 * @retval  module state
 */
module_state_t get_state_pir(module_pir_t *pir){

	return pir->state;

}

/**
 * @brief  Set the state of sensor pir
 * @param  pir		pointer to module pir structure
 * @param  state	module state to be set
 */
void set_state_pir(module_pir_t *pir, module_state_t state){

	pir->state = state;

}

/**
 * @brief   Get the state of the pir's GPIO Pin
 * @param   pir   pointer to module pir structure
 * @retval  GPIO Pin State
 */
GPIO_PinState get_pir_pin_state(module_pir_t *pir){
	return pir->sensor->pin_state;
}

/**
 * @brief  Set the pir's GPIO Pin state
 * @param  pir	  pointer to module pir structure
 * @param  state  state of pin to be set
 */
void set_pir_pin_state(module_pir_t *pir, GPIO_PinState state){
	pir->sensor->pin_state = state;
}




