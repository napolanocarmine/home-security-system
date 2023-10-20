/*
 * KeypadHandler.c
 *
 *  Created on: Jun 1, 2020
 *      Author: CARMINE
 */

#include "keypad.h"
#include "string.h"
#include "system.h"
#include "configuration_protocol.h"
#include "keypad_handler.h"

/**
 * @brief #Cycle for check stability of a pressed key
 */
#define CYCLES_FOR_STABILITY (100)

/**
 * @brief   Read the GPIO_Pin state
 * @param   GPIO_Pin		the associated pin to the keypad button
 * @retval  GPIO_PinState   the read state of the passed GPIO_Pin in input
 * @note    It is used to read the pin and check the stability of the read pin
 */
GPIO_PinState read_pin(uint16_t GPIO_Pin){

	GPIO_PinState currentState,prevState;

	currentState = HAL_GPIO_ReadPin(R_PORT, GPIO_Pin);

	prevState = currentState;

	for(int i = 0; i < CYCLES_FOR_STABILITY; i++){
		currentState = HAL_GPIO_ReadPin(GPIOC, GPIO_Pin);
		if(currentState != prevState){
			prevState = currentState;
			i = 0;
		}
	}

	return prevState;

}

/**
 * @brief  Check if the buffer content is correct
 * @param  buffer	        pointer to command buffer
 * @retval command status
 * @note   It calls:
 * 				-   check_user_pin() to check if the inserted user pin is correct,
 * 				-   check_command() to check if the inserted command is correct
 */
int8_t check_buffer(uint8_t *buffer){

	if(check_user_pin(buffer) == WRONG_USER_PIN)
		return WRONG_USER_PIN;
	else
		return check_command(buffer+PIN_SIZE);

}


/**
 * @brief   Check if the pin in the buffer is correct
 * @param   pin   pointer to user pin buffer, inserted by the user through the keypad
 * @retval  pin status
 * @note    It checks if the inserted pin corresponds to the configurated pin
 * 			It returns the pin status:
 * 					- if the check fails it returns WRONG_USER_PIN,
 * 					- else USER_PIN_OK
 */
int8_t check_user_pin(uint8_t *pin){

	uint8_t user_pin[PIN_SIZE];

	get_user_pin(user_pin); // get the current user pin

	uint8_t read_user_pin[PIN_SIZE];

	for(int i = 0; i < PIN_SIZE; i++){ // take the user pin inserted through keypad
		read_user_pin[i] = pin[i+1];
	}

	if(strncmp((char *)user_pin,(char *)read_user_pin,PIN_SIZE) != 0){
		return WRONG_USER_PIN;
	}

	return USER_PIN_OK;

}

/**
 * @brief   Check if the command in the buffer is correct
 * @param   pointer to command buffer
 * @retval  command status
 * @note    It checks if the inserted command is correct
 * 			It returns the command status:
 * 					- if the check fails it returns COMMAND_REJECTED
 * 					- else COMMAND_ACCEPTED
 */
int8_t check_command(uint8_t *command){

	if(strcmp((char *)command, ACTIVE_AREA_ALLARM) || strcmp((char *)command, ACTIVE_BARRIER_ALLARM) || strcmp((char *)command, ACTIVE_BOTH_ALLARM)
		|| strcmp((char *)command, ACTIVE_SYSTEM) || strcmp((char *)command, DISABLE_AREA_ALLARM) || strcmp((char *)command, DISABLE_BARRIER_ALLARM)
		|| strcmp((char *)command, DISABLE_BOTH_ALLARM) || strcmp((char *)command, DISABLE_SYSTEM)){
		return COMMAND_ACCEPTED;
	}else{
		return COMMAND_REJECTED;
	}

}


