/*
 * handle_timer.c
 *
 *  Created on: May 24, 2020
 *      Author: 2017
 */


#include <timer_handler.h>
#include "usart.h"
#include "system_led.h"
#include "system.h"

/**
 * @brief   Set the timer with the given period
 * @param   timer pointer to the HAL timer peripheral structure
 * @param   period timer period to be set
 * @return  void
 */
void set_timer_period(TIM_HandleTypeDef *timer, uint32_t period){

	__HAL_TIM_SET_AUTORELOAD(timer, period); //set period

}

/**
 * @brief   Start the timer in non-blocking mode
 * @param   timer pointer to the HAL timer peripheral structure
 * @return  void
 */
void start_timer_IT(TIM_HandleTypeDef *timer){

	reset_timer_counter(timer); // reset the timer CNT register

	HAL_TIM_Base_Start_IT(timer);

}

/**
 * @brief   Stop the timer in non-blocking mode
 * @param   timer pointer to the HAL timer peripheral structure
 * @return  void
 */
void stop_timer_IT(TIM_HandleTypeDef *timer){

	reset_timer_counter(timer); // reset the timer CNT register

	HAL_TIM_Base_Stop_IT(timer);

}

/**
 * @brief   Reset the timer CNT register
 * @param   timer pointer to the HAL timer peripheral structure
 * @return  void
 */
void reset_timer_counter(TIM_HandleTypeDef *timer){

	__HAL_TIM_SET_COUNTER(timer,0);

}


int pending_bit=0;

/**
 * @brief   Period Elpased callback redefinition
 * @param   timer pointer to the HAL timer peripheral structure
 * @return  void
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance == TIM10 && system.protocol->state != END_CUSTOM && system.protocol->state != END_DEFAULT){ // check if the protocol is running
		HAL_UART_Abort_IT(&huart2); // abort uart transmission and receiving operations
		system.protocol->state = END_DEFAULT; // set the protocol' state to END_DEFAULT
		stop_timer_IT(system.protocol->timer); // stop the timer
	}
	else if(htim->Instance == TIM10 && system.system_log->state == START_L ){
		// check if the system_log is running
		start_send_log_message(system.system_log); // start the system_log send message procedure

	}
	else if(htim->Instance == TIM11){
		if(pending_bit==1){
			if(system.state != SYSTEM_ALARMED && (get_state_pir(system.pir) == SENSOR_ALARMED || get_state_barrier(system.barrier) == SENSOR_ALARMED)){ // delay time elapsed
				// check if the system is not alarmed and one of the two sensor is alarmed, this represent delay time elapsed
				stop_timer_IT(&htim11); // stop time
				if(get_state_pir(system.pir) == SENSOR_ALARMED && get_state_barrier(system.barrier) == SENSOR_ALARMED)// delay time is elapsed and both modules are alarmed
					alarm_system(&system, BOTH_PULSE);
				else if(get_state_pir(system.pir) == SENSOR_ALARMED) // delay time for pir elapsed
					alarm_system(&system, system.pir->pulse);
				else if(get_state_barrier(system.barrier) == SENSOR_ALARMED)// delay time for barrier elapsed
					alarm_system(&system, system.barrier->pulse);

			}
			else if(system.state == SYSTEM_ALARMED){ // duration time elapsed
				// check if the system state is ALARMED
				stop_timer_IT(&htim11);
				dealarm_system(&system); // call dealarm procedure
			}
			//pending_bit=0;
		}else pending_bit=1;
	}
	else if(htim->Instance == TIM1 && get_state_pir(system.pir) == SENSOR_ACTIVE){ // pir stability signal timer elapsed
		stop_timer_IT(&htim1);
		alarm_pir(system.pir); // call the alarm pir procedure
	}
	else if(htim->Instance == TIM2 && system.state == SYSTEM_ACTIVE){ // toggle led if the system is active
		toggle_system_led();
	}
}

