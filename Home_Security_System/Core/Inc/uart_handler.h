/*
 * uart_handler.h
 *
 *  Created on: May 28, 2020
 *      Author: 2017
 */

#ifndef INC_UART_HANDLER_H_
#define INC_UART_HANDLER_H_

#include "stm32f4xx.h"

/**
 * Define UART_HANDLER Error
 */
#define UART_OK (0)
#define UART_ERR (1)

/**
 * Define UART_Handler structure
 */
struct uart_handler_s{

	UART_HandleTypeDef *huart;

};

typedef struct uart_handler_s uart_handler_t;


/**
 * Initialize the global uart_handler variable with the given reference to the huart.
 */
void uart_handler_init(uart_handler_t *uart_handler, UART_HandleTypeDef *huart);

/**
 * Send buffer_size data of buffer through UART, in DMA mode
 */
int8_t uart_handler_send_message_IT(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size);

/**
 * Send buffer_size data of buffer through UART, in DMA mode
 */
int8_t uart_handler_send_message_DMA(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size);

/**
 * Send buffer_size data if buffer through UART, in blocking mode
 */
int8_t uart_handler_send_message(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size);

/**
 * Receive buffer_size data, inserted into buffer, through UART, in interrupt mode
 */
int8_t uart_handler_receive_message_IT(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size);

/**
 * Receive buffer_size data, inserted into buffer, through UART, in DMA mode
 */
int8_t uart_handler_receive_message_DMA(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size);


/**
 * Receive buffer_size data, inserted into buffer, through UART, in blocking mode
 */
int8_t uart_handler_receive_message(uart_handler_t *uart_handler, uint8_t *buffer, int16_t buffer_size);

#endif /* INC_UART_HANDLER_H_ */
