/*
 * uart_handler.c
 *
 *  Created on: May 28, 2020
 *      Author: 2017
 */


#include <configuration_protocol.h>
#include "uart_handler.h"
#include <stdint.h>
#include "string.h"
#include "system.h"
#include "system_log.h"


/**
 * @brief 	Initialize the uart_handler structure
 * @param 	uart_handler pointer to the uart_handler structure
 * @param 	huart pointer to the HAL Uart_HandleTypeDef that is wrapped by uart_handler
 * @return 	void
 */
void uart_handler_init(uart_handler_t *uart_handler, UART_HandleTypeDef *huart){

	uart_handler->huart = huart;
}

/**
 * @brief 	Send buffer with buffer_size through the uart managed by uart_handler in non-blocking mode
 * @param 	uart_handler pointer to the uart_handler structure
 * @param 	buffer data to send over uart
 * @param 	buffer_size amount of bytes to send
 * @return 	the operation result
 */
int8_t uart_handler_send_message_IT(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size){

	HAL_StatusTypeDef returnValue;

	returnValue = HAL_UART_Transmit_IT(uart_handler->huart, buffer, buffer_size);

	if(returnValue == HAL_OK){
		return UART_OK;
	}

	return UART_ERR;
}

/**
 * @brief 	Send buffer with buffer_size through the uart managed by uart_handler in DMA mode
 * @param 	uart_handler pointer to the uart_handler structure
 * @param 	buffer data to send over uart
 * @param 	buffer_size amount of bytes to send
 * @return 	the operation result
 */
int8_t uart_handler_send_message_DMA(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size){

	HAL_StatusTypeDef returnValue;

	returnValue = HAL_UART_Transmit_DMA(uart_handler->huart, buffer, buffer_size);

	if(returnValue == HAL_OK){
		return UART_OK;
	}

	return UART_ERR;
}

/**
 * @brief 	Send buffer with buffer_size through the uart managed by uart_handler, in blocking-mode
 * @param 	uart_handler pointer to the uart_handler structure
 * @param 	buffer data to send over uart
 * @param 	buffer_size amount of bytes to send
 * @return 	the operation result
 */
int8_t uart_handler_send_message(uart_handler_t *uart_handler,uint8_t *buffer, int16_t buffer_size){

	HAL_StatusTypeDef returnValue;

	do{
		returnValue = HAL_UART_Transmit(uart_handler->huart, buffer, buffer_size, HAL_MAX_DELAY);
	}while(returnValue == HAL_BUSY);

	if(returnValue == HAL_OK){
		return UART_OK;
	}

	return UART_ERR;

}

/**
 * @brief 	Receive buffer_size bytes from uart peripheral in DMA mode
 * @param 	uart_handler pointer to the uart_handler structure
 * @param 	buffer pointer to the array where put the received bytes
 * @param 	buffer_size amount of bytes to be received
 * @return 	the operation result
 */
int8_t uart_handler_receive_message_IT(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size){

	HAL_StatusTypeDef returnValue;

	returnValue = HAL_UART_Receive_IT(uart_handler->huart, buffer, buffer_size);

	if(returnValue == HAL_OK){
		return UART_OK;
	}

	return UART_ERR;

}

/**
 * @brief 	Receive buffer_size bytes from uart peripheral in DMA mode
 * @param 	uart_handler pointer to the uart_handler structure
 * @param 	buffer pointer to the array where put the received bytes
 * @param 	buffer_size amount of bytes to be received
 * @return 	the operation result
 */
int8_t uart_handler_receive_message_DMA(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size){

	HAL_StatusTypeDef returnValue;

	returnValue = HAL_UART_Receive_DMA(uart_handler->huart, buffer, buffer_size);

	if(returnValue == HAL_OK){
		return UART_OK;
	}

	return UART_ERR;

}

/**
 * @brief 	Receive buffer_size bytes from uart peripheral in blocking mode
 * @param 	uart_handler pointer to the uart_handler structure
 * @param 	buffer pointer to the array where put the received bytes
 * @param 	buffer_size amount of bytes to be received
 * @return 	the operation result
 */
int8_t uart_handler_receive_message(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size){

	HAL_StatusTypeDef returnValue;

	returnValue = HAL_UART_Receive(uart_handler->huart, buffer, buffer_size,HAL_MAX_DELAY);

	if(returnValue == HAL_OK){
		return UART_OK;
	}

	return UART_ERR;

}

/**
 * @brief 	Transmission Completed Callback redefinition.
 * @param 	huart pointer to the uart peripheral structure that has raised the interrupt
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){

	if(system.protocol->state != END_CUSTOM && system.protocol->state != END_DEFAULT){
		// check if the protocol is in execution
		protocol_callback_tx();
	}
	else if(system.system_log->state != START_L){
		// check if the system_log is waiting for sending a new part of the message
		log_callback_tx();
	}

}

/**
 * @brief 	Rx Completed Callback redefinition.
 * @param 	huart pointer to the uart peripheral structure that has raised the interrupt
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	protocol_callback_rx(); // call the protocol function that handles the receiving callback.

}
