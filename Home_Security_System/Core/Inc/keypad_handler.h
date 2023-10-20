/*
 * keypad_handler.h
 *
 *  Created on: Jun 3, 2020
 *      Author: 2017
 */

#ifndef INC_KEYPAD_HANDLER_H_
#define INC_KEYPAD_HANDLER_H_


/**
 * Buffer array of a command
 */
uint8_t command_buffer[COMMAND_BUFFER_SIZE];

/**
 * Read the GPIO_Pin state
 */
GPIO_PinState read_pin(uint16_t GPIO_Pin);

/**
 * Check if the buffer content is correct
 */
int8_t check_buffer(uint8_t* buffer);

/**
 * Check if the pin in the buffer is correct
 */
int8_t check_user_pin(uint8_t *pin);

/**
 * Check if the command in the buffer is correct
 */
int8_t check_command(uint8_t *command);

#endif /* INC_KEYPAD_HANDLER_H_ */
