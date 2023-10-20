/*
 * keypad.h
 *
 *  Created on: 31 mag 2020
 *      Author: CARMINE
 */


#include "gpio.h"
#include "usart.h"

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

/*
 * output pin -> colonne
 */
#define C_PORT GPIOB

#define C1_PIN GPIO_PIN_2
#define C2_PIN GPIO_PIN_3
#define C3_PIN GPIO_PIN_4
#define C4_PIN GPIO_PIN_5

/*
 * input pin -> righe
 */
#define R_PORT GPIOC
#define R1_PIN GPIO_PIN_10
#define R2_PIN GPIO_PIN_11
#define R3_PIN GPIO_PIN_12
#define R4_PIN GPIO_PIN_9

#define KEYPAD_READ_INTERVAL 10000
#define KEYPAD_Type_Large 16
#define KEYPAD_NO_PRESSED (uint8_t)0xFF


/*
 * Define Keypad button type
 */
typedef enum{
	KEYPAD_button_0='0',
	KEYPAD_button_1='1',
	KEYPAD_button_2='2',
	KEYPAD_button_3='3',
	KEYPAD_button_4='4',
	KEYPAD_button_5='5',
	KEYPAD_button_6='6',
	KEYPAD_button_7='7',
	KEYPAD_button_8='8',
	KEYPAD_button_9='9',
	KEYPAD_button_STAR='*',
	KEYPAD_button_HASH='#',
	KEYPAD_button_A='A',
	KEYPAD_button_B='B',
	KEYPAD_button_C='C',
	KEYPAD_button_D='D',
	KEYPAD_button_NOPRESSED = KEYPAD_NO_PRESSED,
} KEYPAD_button_t;

/**
 * Initialize the Keypad
 */
void KEYPAD_init();

/**
 * Read the right column of keypad
 */
void KEYPAD_read_colum(uint8_t column);

/**
 * Read the right row of a keypad
 */
char KEYPAD_find_button(uint16_t R_PIN);

/**
 * Update the keypad status with the last button pressed
 */
KEYPAD_button_t KEYPAD_Update(uint16_t GPIO_Pin);

/**
 * Set columns to high value
 */
void set_colums();

/**
 * Reset columns to low value
 */
void reset_colums();

#endif /* INC_KEYPAD_H_ */
