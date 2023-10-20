/*
 * module_barrier.c
 *
 *  Created on: 1 Jun 2020
 *      Author: 2017
 */


#include "module_barrier.h"
#include "system.h"
#include "adc.h"

/**
 * @brief Number of read samples over the threshold to check stability signal
 */
uint32_t counter;

/**
 * @brief  Initialize module barrier
 * @param  module_barrier   pointer to module barrier structure
 * @param  state		    barrier state, it can assume the following value:
 * 								-   SENSOR_ACTIVE,
								-   SENSOR_INACTIVE,
								- 	SENSOR_ALARMED,
								-	SENSOR_DELAYED
 * @param  photoresistor   pointer to photoresistor structure
 * @param  laser		   pointer to laser structure
 * @param  delay		   alarm delay value
 * @param  pulse		   ringtone value
 * @param  stable_signal   number of conversions under the threshold to consider the signal stable
 * @note   The module sets its threshold:
 * 		   		-  the threshold_up is the value without laser
 * 		   		-  the treshold_down is the value with laser
 * 		   		-  the final threshold is the mean of the above values
 */
void init_module_barrier(module_barrier_t *module_barrier,module_state_t state, photoresistor_t *photoresistor, laser_t *laser, uint8_t delay, uint16_t pulse, uint32_t stable_signal){

	module_barrier->state = state;

	module_barrier->photoresistor = photoresistor;

	module_barrier->laser = laser;

	module_barrier->pulse = pulse;

	module_barrier->delay = delay;

	set_threshold(module_barrier);

	module_barrier->stable_signal = stable_signal;

	if(state == SENSOR_INACTIVE)
		reset_laser(laser);

}

/**
 * @brief  Set the threshold
 * @param  module_barrier  pointer to module barrier structure
 * @note   		-  the threshold_up is the value without laser
 * 		   		-  the treshold_down is the value with laser
 * 		   		-  the final threshold is the mean of the above values
 */
void set_threshold(module_barrier_t *module_barrier){

	module_barrier->threshold_up = read_value(module_barrier->photoresistor); //threshold without laser

	set_laser(module_barrier->laser);

	HAL_Delay(1000);

	module_barrier->threshold_down = read_value(module_barrier->photoresistor); //threshold with laser

	module_barrier->threshold = (module_barrier->threshold_down + module_barrier->threshold_up)/2;
}

/**
 * @brief   Get barrier state
 * @param   module_barrier  pointer to module barrier structure
 * @retval  module state
 */
module_state_t get_state_barrier(module_barrier_t *module_barrier){

	return module_barrier->state;

}

/**
 * @brief  Set barrier state
 * @param  module_barrier  pointer to module barrier structure
 * @param  state 		   barrier state
 */
void set_state_barrier(module_barrier_t *module_barrier, module_state_t state){

	module_barrier->state = state;

	if(state == SENSOR_ACTIVE){
		start_barrier_sensor(module_barrier);
	}else if(state == SENSOR_INACTIVE){
		stop_barrier_sensor(module_barrier);
	}

}

/**
 * @brief  Start the barrier sensor to capture brightness variation
 * @param  module_barrier  pointer to module barrier structure
 * @note   Set the laser and start the photoresistor ADC sequence conversion
 */
void start_barrier_sensor(module_barrier_t *module_barrier){

	set_laser(module_barrier->laser);
	start_read_value_IT(module_barrier->photoresistor);

}

/**
 * @brief  Stop the barrier sensor (deactivate it)
 * @param  module_barrier  pointer to module barrier structure
 * @note Reset the laser and stop the photoresistor ADC sequence conversion
 */
void stop_barrier_sensor(module_barrier_t *barrier){

	reset_laser(barrier->laser);
	stop_read_value_IT(barrier->photoresistor);

}

/**
 * @brief  Redefinition of the ADC conversion completed callback
 * @param  hadc adc handler
 * @note   Read the raw value and compare it with the threshold.
 * 		   Increment the counter if the rawvalue is over the threshold:
 * 		   	 -	if the number are over the stability value (stable_signal), it starts the alarm
 * 		   	 -  else it resets the counter
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

	uint16_t rawValue = 0;

	if(hadc -> Instance == ADC1){
		rawValue = HAL_ADC_GetValue(&hadc1);;
		if((rawValue > system.barrier->threshold) && get_state_barrier(system.barrier) != SENSOR_ALARMED){
			counter+=1;
			if(counter > system.barrier->stable_signal){ // check the barrier signal stability:
				counter = 0;
				stop_read_value_IT(system.barrier->photoresistor);
				alarm_barrier(system.barrier); // alarm barrier sensor

			}
		}else if(counter != 0){
			counter = 0;
		}

	}

}
