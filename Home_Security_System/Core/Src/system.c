/*
 * system.c
 *
 *  Created on: May 26, 2020
 *      Author: emiso
 */

#include "configuration_protocol.h"
#include "stm32f4xx.h"
#include "i2c.h"
#include "system.h"
#include "keypad.h"
#include "timer_handler.h"
#include "system_led.h"
#include "adc.h"
#include "keypad_handler.h"
#include "uart_handler.h"
#include "string.h"

/**
 * @brief String for wrong configuration
 */
#define INITIALIZATION_ERROR ("\n\rINITIALIZATION ERROR: CHECK CONNECTIONS\n\r")

/**
 * @brief String for a wrong inserted USER PIR
 */
#define WRONG_USER_PIN_MESSAGE ("WRONG USER PIN\n\r")

/**
 * @brief String for command accepted message
 */
#define COMMAND_ACCEPTED_MESSAGE ("COMMAND ACCEPTED\n\r")

/**
 * @brief String for command rejected message
 */
#define COMMAND_REJECTED_MESSAGE ("COMMAND REJECTED\n\r")

/**
 * @brief Length of wrong user pin message
 */
#define WRONG_USER_PIN_LENGTH (18)

/**
 * @brief Length of command accepted message
 */
#define COMMAND_ACCEPTED_LENGTH (20)

/**
 * @brief Length of command rejected message
 */
#define COMMAND_REJECTED_LENGTH (20)

/**
 * @brief GPIO Port of the sensor Pir
 */
#define PIR_SENSOR_PORT (GPIOA)

/**
 * @brief GPIO Pin of the sensor Pir
 */
#define PIR_SENSOR_PIN (GPIO_PIN_7)

/**
 * @brief GPIO Port of the laser
 */
#define LASER_PORT (GPIOA)

/**
 * @brief GPIO Pin of the laser
 */
#define LASER_PIN (GPIO_PIN_1)

/**
 * @brief minimum time value in milliseconds for the pir signal stability
 */
#define SIGNAL_STABILITY_S (999)

/**
 * @brief minimum number of samples for the barrier signal stability
 */
#define SIGNAL_STABILITY_B (16393)

/**
 * @brief Global system variable
 */
system_t system;

/**
 * @brief Global digital sensor variable
 */
digital_sensor_t sensor_pir;

/**
 * @brief Global pir variable
 */
module_pir_t pir;

/**
 * @brief Global laser variable
 */
laser_t laser;

/**
 * @brief Global photoresistor variable
 */
photoresistor_t photoresistor;

/**
 * @brief Global barrier variable
 */
module_barrier_t barrier;

/**
 * @brief Global buzzer variable
 */
buzzer_t buzzer;

/**
 * @brief Global uart handler variable
 */
uart_handler_t uart_handler;

/**
 * @brief Global rtc variable
 */
rtc_t rtc;

/**
 * @brief Global system log variable
 */
system_log_t system_log;

/**
 * @brief Global system configuration variable
 */
system_configuration_t configuration;

/**
 * @brief Global configuration protocol variable
 */
configuration_protocol_t protocol;

/**
 * @brief Global cnt variable: #inserted character in command buffer
 */
uint8_t cnt=0;

/**
 * @brief  Initialize the system
 * @return operation result
 * @note   It is called by the main at the start of the system
 * 				-  set system state to SYSTEM_INACTIVE,
 * 		   		-  turn on the system led,
 * 		   		-  initialize all the support elements (uart, rtc, system log, protocol),
 * 		   		-  configure the protocol,
 * 		   		-  initialize all the sensor (pir, barrier, laser, photoresistor, buzzer)
 */
int8_t init_system(){

	system.state = SYSTEM_INACTIVE;

	turn_on_system_led();

	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

	if(init_elements() == SYS_OK){ // initialize all the support elements

		configuration_protocol(&protocol); // start configuration protocol

		system.system_configuration = &configuration; // assign the configuration produced

		init_sensor(&pir, &sensor_pir, &barrier, &laser, &photoresistor, &buzzer); // initilize all the sensors

		system.pir = &pir;
		system.buzzer = &buzzer;
		system.barrier = &barrier;

		return SYS_OK; // return System OK
	}else
		return SYS_ERR; // return System error

}

/**
 * @brief Run the system
 * @note  It is called by the main at the end of initialization
 * 			 -  initialize the keypad,
 * 			 -  start the system log
 */
void run_system(){

	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	KEYPAD_init();
	start_system_log(&system_log);

}

/**
 * @brief  Initialize all the support elements
 * @return operation result
 * @note   It is called by init_system(). It initializes:
 * 				-  uart handler,
 * 				-  rtc,
 * 				-  system log,
 * 				-  protocol
 */
int8_t init_elements(){

	uart_handler_init(&uart_handler, &huart2); // initialize uart_handler;

	if(ds1307rtc_init(&rtc, &hi2c1) != DS1307_ERR){// initialize rtc;

		init_system_log(&system_log, &rtc, &uart_handler, &htim10); // initialize system_log module;

		init_protocol(&protocol, &configuration, &htim10, &rtc); // initialize configuration protocol module;

		system.rtc = &rtc;

		system.system_log = &system_log;

		system.protocol = &protocol;

		return SYS_OK;
	}else{
		uart_handler_send_message(&uart_handler, (uint8_t *)INITIALIZATION_ERROR, strlen(INITIALIZATION_ERROR));
		return SYS_ERR;
	}


}

/**
 * @brief  Initialize all the sensors
 * @param  pir				pointer to module pir structure
 * @param  sensor_pir		pointer to digital sensor pir structure
 * @param  barrier			pointer to module barrier structure
 * @param  laser			pointer to laser structure
 * @param  photoresistor	pointer to photoresistor structure
 * @param  buzzer			pointer to buzzer structure
 * @note   It is called by init_system(). It initializes:
 * 				-  pir
 * 				-  buzzer
 * 				-  laser
 * 				-  photoresistor
 * 				-  barrier
 */
void init_sensor(module_pir_t *pir, digital_sensor_t* sensor_pir,module_barrier_t *barrier, laser_t *laser, photoresistor_t *photoresistor, buzzer_t *buzzer){

	GPIO_PinState pir_state = HAL_GPIO_ReadPin(PIR_SENSOR_PORT, PIR_SENSOR_PIN); // read the sensor state
	init_pir(pir, sensor_pir, pir_state, PIR_SENSOR_PORT, PIR_SENSOR_PIN, SENSOR_INACTIVE, system.system_configuration->sensor_delay_1, &htim1, PIR_PULSE);
	init_buzzer(buzzer, BUZZER_INACTIVE, &htim3);
	init_laser(laser, LASER_PORT, LASER_PIN, GPIO_PIN_RESET);
	init_photoresistor(photoresistor, &hadc1);
	init_module_barrier(barrier, SENSOR_INACTIVE, photoresistor, laser, system.system_configuration->sensor_delay_2, BARRIER_PULSE, SIGNAL_STABILITY_B);

}

/**
 * @brief   Activate the system
 * @param   system	 pointer to system structure
 * @return  command status
 * @note    When the system is active the led toggles
 */
int8_t activate_system(system_t *system){


	if(system->state != SYSTEM_ACTIVE){ // if the system isn't already active
		system->state = SYSTEM_ACTIVE; // active system
		start_timer_IT(&htim2); // start blinking led
		return COMMAND_EXECUTED;
	}

	return COMMAND_ERROR;
}

/**
 * @brief   Deactivate the system
 * @param   system	 pointer to system structure
 * @return  command status
 * @note    When the system is inactive the led is turn on
 */
int8_t deactivate_system(system_t *system){

	if(system->state != SYSTEM_INACTIVE && deactivate_both(system) == COMMAND_EXECUTED){ // the system is ACTIVE or ALARMED
		turn_on_system_led();
		system->state = SYSTEM_INACTIVE;
		return COMMAND_EXECUTED;
	}
	return COMMAND_ERROR;
}

/**
 * @brief  Alarm the system
 * @param  system	pointer to system structure
 * @param  pulse    pulse value
 * @note   If the system is not alarmed, this function alarm it.
 * 		   It activates the buzzer with the given pulse.
 */
void alarm_system(system_t *system, uint16_t pulse){

	stop_timer_IT(&htim2);
	turn_on_system_led();
	if(system->state == SYSTEM_ACTIVE){ // the system is not emitting any sound and the delay time is elapsed.
		// the system sets the duration timer, when its state is ACTIVE.
		system->state = SYSTEM_ALARMED;
		reset_timer_counter(&htim11);
		set_timer_period(&htim11, (system->system_configuration->duration *1000)-1); // duration time
		start_timer_IT(&htim11);
		activate_buzzer(system->buzzer, pulse);

	} // the function is called by another module, while the system is emitting the alarm for the other one.
	else if(system->state == SYSTEM_ALARMED && pulse == BOTH_PULSE){ // the system is emitting the sound for one of the two modules

		reset_timer_counter(&htim11); // reset duration timer
		set_timer_period(&htim11, (system->system_configuration->duration *1000)-1); // set duration period
		start_timer_IT(&htim11); // restart timer
		activate_buzzer(system->buzzer, BOTH_PULSE); // active buzzer with BOTH_PULSE

	}
}

/**
 * @brief  Dealarm the system
 * @param  system	pointer to system structure
 * @note   Set the system state in SYSTEM_ACTIVE and deactivate the buzzer.
 * 		   Set SENSOR_ACTIVE state if a module is alarmed.
 */
void dealarm_system(system_t *system){

	deactivate_buzzer(&buzzer); // stop buzzer

	activate_system(system); // set the system state to active

	if(get_state_barrier(system->barrier) == SENSOR_ALARMED) // check if the barrier is allarmed
		activate_module_barrier(system); // set the barrier state to active
	if(get_state_pir(system->pir) == SENSOR_ALARMED){

		activate_module_pir(system); // set the pir state to active

		if(HAL_GPIO_ReadPin(PIR_SENSOR_PORT, PIR_SENSOR_PIN) == GPIO_PIN_SET){
			start_timer_IT(&htim1); // the sensor pin is still high, restart timer
		}

	}

}


/**
 * @brief   Activate the pir sensor
 * @param   system	 pointer to system structure
 * @return  command status
 */
int8_t activate_module_pir(system_t *system){

	if(system->state == SYSTEM_ACTIVE && get_state_pir(system->pir) != SENSOR_ACTIVE){ // check if the system is active
		set_state_pir(system->pir, SENSOR_ACTIVE); // active pir
		return COMMAND_EXECUTED;
	}
	return COMMAND_ERROR;
}


/**
 * @brief   Deactivate the pir sensor
 * @param   system	 pointer to system structure
 * @return  command status
 */
int8_t deactivate_module_pir(system_t *system){

	if(system->state != SYSTEM_INACTIVE){ // the system is ACTIVE or ALARMED

		if(get_state_pir(system->pir) == SENSOR_ALARMED && get_state_barrier(system->barrier) != SENSOR_ALARMED){ // the sensor is waiting for delay or the buzzer is emitting the alarm
			stop_timer_IT(&htim11); // stop delay/duration timer

			if(get_state_buzzer(system->buzzer) == BUZZER_ACTIVE){
				deactivate_buzzer(&buzzer); // stop alarm
			}
			set_state_pir(system->pir, SENSOR_INACTIVE); // deactivate pir
			if(system->state == SYSTEM_ALARMED) // reactive the system.
				activate_system(system);
			return COMMAND_EXECUTED;

		}else if(get_state_pir(system->pir) == SENSOR_ACTIVE){
			set_state_pir(system->pir, SENSOR_INACTIVE); // deactivate pir
			if(get_state_barrier(system->barrier) != SENSOR_ALARMED)
				activate_system(system);
			return COMMAND_EXECUTED;
		}
	}
	return COMMAND_ERROR;

}

/**
 * @brief  Alarm the pir sensor
 * @param  system	pointer to pir structure
 */
void alarm_pir(module_pir_t *pir){

	set_state_pir(pir, SENSOR_ALARMED);

	if(get_state_barrier(system.barrier) != SENSOR_ALARMED){ // check if the barrier isn't waiting for delay or the barrier alarm isn't been emitting
		// The barrier is INACTIVE or ACTIVE.
		// It's possible to command delay timer
		// set the delay timer or alarm the system if any delay has been set.
		if(pir->delay > 0){
			reset_timer_counter(&htim11);
			set_timer_period(&htim11, (pir->delay * 1000)-1); //delay sensor
			start_timer_IT(&htim11);
		}
		else
			alarm_system(&system,system.pir->pulse);

	}else if (system.state == SYSTEM_ALARMED) // the alarm is been emitting.
		alarm_system(&system,BOTH_PULSE);

}

/**
 * @brief   Activate the barrier sensor
 * @param   system	 pointer to system structure
 * @return  command status
 */
int8_t activate_module_barrier(system_t *system){

	if(system->state == SYSTEM_ACTIVE && get_state_barrier(system->barrier) != SENSOR_ACTIVE){ // check if the system is active
		set_state_barrier(system->barrier, SENSOR_ACTIVE); // active barrier
		return COMMAND_EXECUTED;
	}
	return COMMAND_ERROR;

}

/**
 * @brief   Deactivate the barrier sensor
 * @param   system	 pointer to system structure
 * @return  command status
 */
int8_t deactivate_module_barrier(system_t *system){

	if(system->state != SYSTEM_INACTIVE){ // the system is ACTIVE or ALARMED

		if(get_state_barrier(system->barrier) == SENSOR_ALARMED && get_state_pir(system->pir) != SENSOR_ALARMED){ // the sensor is waiting for delay or the buzzer is emitting the alarm
			stop_timer_IT(&htim11); // stop delay/duration timer
			if(get_state_buzzer(system->buzzer) == BUZZER_ACTIVE){
				deactivate_buzzer(&buzzer); // stop alarm
			}
			set_state_barrier(system->barrier, SENSOR_INACTIVE ); // deactivate barrier
			if(system->state == SYSTEM_ALARMED)
				activate_system(system);
			return COMMAND_EXECUTED;

		}else if(get_state_barrier(system->barrier) == SENSOR_ACTIVE){

			set_state_barrier(system->barrier, SENSOR_INACTIVE ); // deactivate barrier
			if(get_state_pir(system->pir) != SENSOR_ALARMED)
				activate_system(system);
			return COMMAND_EXECUTED;
		}
	}

	return COMMAND_ERROR;

}

/**
 * @brief  Alarm the barrier sensor
 * @param  system	pointer to barrier structure
 */
void alarm_barrier(module_barrier_t *barrier){

	set_state_barrier(barrier, SENSOR_ALARMED);

	if(get_state_pir(system.pir) != SENSOR_ALARMED){ // check if the barrier isn't waiting for delay or the barrier alarm isn't been emitting
		// The barrier is INACTIVE or ACTIVE.
		// It's possible to command delay timer
		// set the delay timer or alarm the system if any delay has been set.
		if(barrier->delay > 0){
			reset_timer_counter(&htim11);
			set_timer_period(&htim11, (barrier->delay * 1000)-1); //delay sensor
			start_timer_IT(&htim11);
		}
		else
			alarm_system(&system, system.barrier->pulse);

	}
	else if (system.state == SYSTEM_ALARMED) // the alarm is been emiting.
		alarm_system(&system,BOTH_PULSE);

}

/**
 * @brief   Dectivate both sensor
 * @param   system	 pointer to system structure
 * @return  command status
 */
int8_t deactivate_both(system_t *system){

	if(system->state != SYSTEM_INACTIVE){

		if(get_state_barrier(system->barrier) == SENSOR_ALARMED || get_state_pir(system->pir) == SENSOR_ALARMED){ //both alarms are alarmed

			stop_timer_IT(&htim11); //stop delay/duration timer

			if(get_state_buzzer(system->buzzer) == BUZZER_ACTIVE){

				deactivate_buzzer(system->buzzer); //stop buzzer alarm

			}
		}
		set_state_pir(system->pir, SENSOR_INACTIVE);
		set_state_barrier(system->barrier, SENSOR_INACTIVE);
		activate_system(system);
		return COMMAND_EXECUTED;
	}
	return COMMAND_ERROR;
}

/**
 * @brief  Get the inserted pin
 * @param  dest		pointer to array
 */
void get_user_pin(uint8_t *dest){
	memcpy(dest, system.system_configuration->pin, PIN_SIZE);
}

/**
 * @brief   Execute the command inserted by user
 * @param   command	 pointer to command buffer
 * @return  command status
 * @note   	It checks what is the corresponding command
 */
int8_t execute_command(uint8_t *command){

	if(strncmp((char *)command, ACTIVE_AREA_ALLARM,COMMAND_SIZE) == 0){

		if(activate_module_pir(&system) == COMMAND_EXECUTED) // execute command
			return COMMAND_ACCEPTED;

	}else if(strncmp((char *)command, ACTIVE_BARRIER_ALLARM,COMMAND_SIZE) == 0){

		if(activate_module_barrier(&system) == COMMAND_EXECUTED) // execute command
			return COMMAND_ACCEPTED;

	}else if(strncmp((char *)command, ACTIVE_BOTH_ALLARM,COMMAND_SIZE) == 0){

		if(activate_module_pir(&system) == COMMAND_EXECUTED && activate_module_barrier(&system) == COMMAND_EXECUTED) // execute command
			return COMMAND_ACCEPTED;

	}else if(strncmp((char *)command, ACTIVE_SYSTEM,COMMAND_SIZE) == 0 ){

		if(activate_system(&system) == COMMAND_EXECUTED) // execute command
			return COMMAND_ACCEPTED;

	}else if(strncmp((char *)command, DISABLE_AREA_ALLARM,COMMAND_SIZE) == 0){

		if(deactivate_module_pir(&system) == COMMAND_EXECUTED) // execute command
			return COMMAND_ACCEPTED;

	}else if(strncmp((char *)command, DISABLE_BARRIER_ALLARM,COMMAND_SIZE) == 0){

		if(deactivate_module_barrier(&system) == COMMAND_EXECUTED)  // execute command
			return COMMAND_ACCEPTED;

	}else if(strncmp((char *)command, DISABLE_BOTH_ALLARM,COMMAND_SIZE) == 0){
		if(deactivate_both(&system) == COMMAND_EXECUTED) // execute command
			return COMMAND_ACCEPTED;

	}else if(strncmp((char *)command, DISABLE_SYSTEM,COMMAND_SIZE) == 0){
		if(deactivate_system(&system) == COMMAND_EXECUTED) // execute command
			return COMMAND_ACCEPTED;

	}

	return COMMAND_REJECTED;
}

/**
 * @brief  Redefinition of EXTI Callback
 * @Param  GPIO_Pin		The GPIO_Pin that generates interrupt
 * @note   It manages the Keypad interrupt and the pir interrupt
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if(GPIO_Pin == PIR_SENSOR_PIN){  //system.pir->sensor.GPIO_Pin

		if((system.state == SYSTEM_ACTIVE || system.state == SYSTEM_ALARMED) && get_state_pir(system.pir) == SENSOR_ACTIVE){
			//check if system is alarmed or active and pir is active
				if(HAL_GPIO_ReadPin(PIR_SENSOR_PORT, PIR_SENSOR_PIN)){ // rising edge
					//pir_rising_edge();
					set_pir_pin_state(system.pir, GPIO_PIN_SET); // update pin state
					set_timer_period(system.pir->timer, SIGNAL_STABILITY_S); // set period for checking signal stability
					reset_timer_counter(system.pir->timer); // reset timer counter
					start_timer_IT(system.pir->timer);
				}else if(!HAL_GPIO_ReadPin(PIR_SENSOR_PORT, PIR_SENSOR_PIN) && get_state_pir(system.pir) != SENSOR_ALARMED){ // falling edge and timer update event not fired
					set_pir_pin_state(system.pir, GPIO_PIN_RESET); // update pin state
					stop_timer_IT(system.pir->timer);
					reset_timer_counter(system.pir->timer);
				}else
					set_pir_pin_state(system.pir, GPIO_PIN_RESET);

		}

	}
	else if((GPIO_Pin == R1_PIN || GPIO_Pin == R2_PIN || GPIO_Pin == R3_PIN || GPIO_Pin == R4_PIN)){

		if(read_pin(GPIO_Pin) == GPIO_PIN_SET){
			KEYPAD_button_t button = KEYPAD_Update(GPIO_Pin);
			//HAL_UART_Transmit(&huart2, &button, 1, HAL_MAX_DELAY);
			if(button == KEYPAD_button_HASH && cnt == 0){ // first valid character

				command_buffer[cnt] = button; // insert the first valid character into the buffer
				cnt +=1 ;

			}else if(command_buffer[0] == KEYPAD_button_HASH && cnt < COMMAND_BUFFER_SIZE && cnt > 0){ // successive command characters

				command_buffer[cnt] = button; // insert the character into the buffer
				cnt += 1;
				if(cnt == COMMAND_BUFFER_SIZE){
					cnt = 0;
					//HAL_UART_Transmit(&huart2, command_buffer, 7, HAL_MAX_DELAY);
					//HAL_UART_Transmit(&huart2, system.system_configuration->pin, PIN_SIZE, HAL_MAX_DELAY);
					if(check_user_pin(command_buffer) != WRONG_USER_PIN){// check the command inserted
						if(execute_command(command_buffer+1+PIN_SIZE) == COMMAND_ACCEPTED){
							system_log_send_message(system.system_log, (uint8_t *) COMMAND_ACCEPTED_MESSAGE, COMMAND_ACCEPTED_LENGTH);
							if(get_state_buzzer(system.buzzer) != BUZZER_ACTIVE){
								activate_buzzer(system.buzzer, COMMAND_PULSE);
							}

						}else{
							system_log_send_message(system.system_log, (uint8_t *) COMMAND_REJECTED_MESSAGE, COMMAND_REJECTED_LENGTH);
						}
					}else{
						system_log_send_message(system.system_log, (uint8_t *) WRONG_USER_PIN_MESSAGE, WRONG_USER_PIN_LENGTH);
					}

				}

			}

		}
	}
}

