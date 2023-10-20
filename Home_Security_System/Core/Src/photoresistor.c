/*
 * photoresistor.c
 *
 *  Created on: Jun 1, 2020
 *      Author: 2017
 */
#include "photoresistor.h"
#include "stm32f4xx.h"


/**
 * @brief  Initialize photoresistor
 * @param  photoresistor	pointer to photoresistor structure
 * @param  hadc				pointer to adc peripheral handler
 */
void init_photoresistor(photoresistor_t *photoresistor, ADC_HandleTypeDef *hadc){

	photoresistor->hadc = hadc;

}

/**
 * @brief   Read a raw value in polling mode
 * @param   photoresistor	pointer to photoresistor structure
 * @retval  rawvalue read
 * @note    It is used to a spot reading during the initialization phase to set the threshold
 */
int16_t read_value(photoresistor_t *photoresistor){

	uint16_t value = -1;

	HAL_ADC_Start(photoresistor->hadc);
	if(HAL_ADC_PollForConversion(photoresistor->hadc, HAL_MAX_DELAY) == HAL_OK){
		 value = HAL_ADC_GetValue(photoresistor->hadc);
	}
	HAL_ADC_Stop(photoresistor->hadc);
	return value;
}

/**
 * @brief  Start reading sequence
 * @param  photoresistor	pointer to photoresistor structure
 * @note Start the ADC interrupt mode
 */
void start_read_value_IT(photoresistor_t *photoresistor){

	HAL_ADC_Start_IT(photoresistor->hadc);

}

/**
 * @brief  Stop reading sequence
 * @param  photoresistor	pointer to photoresistor structure
 * @note Stop the ADC interrupt mode
 */
void stop_read_value_IT(photoresistor_t *photoresistor){

	HAL_ADC_Stop_IT(photoresistor->hadc);

}
