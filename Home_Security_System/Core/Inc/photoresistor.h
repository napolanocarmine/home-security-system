/*
 * photoresistor.h
 *
 *  Created on: 1 Jun 2020
 *      Author: 2017
 */

#ifndef INC_PHOTORESISTOR_H_
#define INC_PHOTORESISTOR_H_
#include "stm32f4xx.h"

/**
 * Define photoresistor struct
 */
typedef struct{

	ADC_HandleTypeDef *hadc;

}photoresistor_t;

/**
 * Initialize photoresistor
 */
void init_photoresistor(photoresistor_t *photoresistor, ADC_HandleTypeDef *hadc);

/**
 * Read rawvalue
 */
int16_t read_value(photoresistor_t *photoresistor);

/**
 * Start reading
 */
void start_read_value_IT(photoresistor_t *photoresistor);

/**
 * Stop reading
 */
void stop_read_value_IT(photoresistor_t *photoresistor);

#endif /* INC_PHOTORESISTOR_H_ */
