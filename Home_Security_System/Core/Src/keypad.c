/*
 * keypad.c
 *
 *  Created on: May 31, 2020
 *      Author: CARMINE
 */

//***** Header files *****//
#include "keypad.h"

#include "keypad.h"
#include "gpio.h"
#include "usart.h"
#include "keypad.h"

/**
 * @brief  keypad matrix scheme
 */
char KEYPAD_INT_Buttons[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'},
};


/**
 * @brief Default Keypad Status
 */
static KEYPAD_button_t KeypadStatus = KEYPAD_button_NOPRESSED ;


/**
 * @brief  Set columns to high value
 */
void set_columns(){
	HAL_GPIO_WritePin(C_PORT,C1_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(C_PORT,C2_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(C_PORT,C3_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(C_PORT,C4_PIN,GPIO_PIN_SET);
}

/**
 * @brief  Set rows to low value
 */
void reset_columns(){
	HAL_GPIO_WritePin(C_PORT,C1_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(C_PORT,C2_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(C_PORT,C3_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(C_PORT,C4_PIN,GPIO_PIN_RESET);
}

/**
 * @brief Initialize the Keypad
 */
void KEYPAD_init(){

	  set_columns();

}

/**
 * @brief  Set the given column to the high level
 * @param  column column_pin
 * @note At first, this function resets all columns then sets the given column
 */
void KEYPAD_read_column(uint8_t column) {

	reset_columns();

	if (column == 1) {
		HAL_GPIO_WritePin(C_PORT,C1_PIN,GPIO_PIN_SET);
	}
	if (column == 2) {
		HAL_GPIO_WritePin(C_PORT,C2_PIN,GPIO_PIN_SET);
	}
	if (column == 3) {
		HAL_GPIO_WritePin(C_PORT,C3_PIN,GPIO_PIN_SET);
	}
	if (column == 4) {
		HAL_GPIO_WritePin(C_PORT,C4_PIN,GPIO_PIN_SET);
	}
}

/**
 * @brief   Find the pressed button
 * @param   R_PIN	 It is the GPIO_PIN of the row corresponding to the button pressed
 * @retval  char represented the pressed button
 * @note    It checks the intersection: when it founds the correct one it returns the char
 * 		    corresponding to the matrix intersection
 */
char KEYPAD_find_button(uint16_t R_PIN) {

	int i = -1;
	if(R_PIN == R1_PIN)
		i = 0;
	if(R_PIN == R2_PIN)
		i = 1;
	if(R_PIN == R3_PIN)
		i = 2;
	if(R_PIN == R4_PIN)
		i = 3;

	KEYPAD_read_column(1);
	if(HAL_GPIO_ReadPin(R_PORT, R_PIN)){
		return KEYPAD_INT_Buttons[i][0];
	}

	KEYPAD_read_column(2);
	if(HAL_GPIO_ReadPin(R_PORT, R_PIN)){
		return KEYPAD_INT_Buttons[i][1];
	}

	KEYPAD_read_column(3);
	if(HAL_GPIO_ReadPin(R_PORT, R_PIN)){
		return KEYPAD_INT_Buttons[i][2];
	}

	KEYPAD_read_column(4);
	if(HAL_GPIO_ReadPin(R_PORT, R_PIN)){
		return KEYPAD_INT_Buttons[i][3];
	}

	return KEYPAD_NO_PRESSED;
}

/**
 * @brief   Update the keypad status with the last button pressed
 * @param   R_PIN	 It is the GPIO_PIN of the row corresponding to the pressed button
 * @retval  KEYPAD_Button_t pressed button
 * @note    It calls the KEYPAD_read_row() to find the char corresponding to the button pressed.
 * 			So it does a casting to KEYPAD_button_t and return the right Keypad button
 */
KEYPAD_button_t KEYPAD_Update(uint16_t R_Pin) {

	KEYPAD_button_t temp;

	KeypadStatus = (KEYPAD_button_t) KEYPAD_find_button(R_Pin); // read the pressed row;

	temp = KeypadStatus;

	set_columns();

	__HAL_GPIO_EXTI_CLEAR_IT(R_Pin);

	KeypadStatus = KEYPAD_button_NOPRESSED;
	return temp;
}

