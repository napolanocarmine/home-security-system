/*
 * system_log.h
 *
 *  Created on: 30 May 2020
 *      Author: 2017
 */

#ifndef INC_SYSTEM_LOG_H_
#define INC_SYSTEM_LOG_H_

#include "timer_handler.h"
#include "stm32f4xx_hal.h"
#include "ds1307rtc.h"
#include "uart_handler.h"

/**
 * Define standard messages
 */
#define SYSTEM_BOOT ("\n\rSYSTEM BOOT ")
#define CUSTOM_CONFIGURATION_LOADED ("\n\rSystem Configuration Loaded\n\r")
#define DEFAULT_CONFIGURATION_LOADED ("\n\rSystem Configuration Rejected\n\r")
#define ERROR_STRING ("\n\rERROR RESTART THE BOARD")


#define ACTIVE_STRING (" ACTIVE ")
#define INACTIVE_STRING (" INACTIVE ")
#define ALLARMED_STRING (" ALLARMED ")

/**
 * Define the system log period
 */
#define SYSTEM_LOG_PERIOD (9999)

/**
 * Define system log status
 */
typedef enum{
	IDLE_L,
	START_L,
	WAITING,
	DATE_TIME_T,
	SYSTEM_STATE_T,
	STOP_L
} log_state_t;

/**
 * Define system log structure
 */
struct system_log_s{

	log_state_t state;
	rtc_t *rtc;
	uart_handler_t *uart;
	TIM_HandleTypeDef *timer;

};

typedef struct system_log_s system_log_t;

/**
 * Initialize system_log
 */
void init_system_log(system_log_t *system_log, rtc_t *rtc, uart_handler_t *uart_handler, TIM_HandleTypeDef *timer);

/**
 * Start the log procedure
 */
void start_system_log(system_log_t *system_log);

/**
 * Stop the log procedure
 */
void stop_system_log(system_log_t *system_log);

/**
 * Start to send the messages for the log
 */
void start_send_log_message(system_log_t *system_log);

/**
 * Perform the action in response to the UART transmission callback
 */
void log_callback_tx();


// Declaration of some module's functions

/**
 * Send the given buffer with the given size through UART, in interrupt mode
 */
int8_t system_log_send_message_IT(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size);

/**
 * Send the given buffer with the given size through UART, in DMA mode
 */
int8_t system_log_send_message_DMA(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size);

/**
 * Send the given buffer with the given size through UART, in blocking mode
 */
int8_t system_log_send_message(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size);


/**
 * Prepare the UART interface for receiving buffer_size data inserted into buffer, in interrupt mode
 */
int8_t system_log_receive_message_IT(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size);

/**
 * Prepare the UART interface for receiving buffer_size data inserted into buffer, in DMA mode
 */
int8_t system_log_receive_message_DMA(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size);


/**
 * Prepare the UART interface for receiving buffer_size data inserted into buffer, in non-blocking mode
 */
int8_t system_log_receive_message(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size);


#endif /* INC_SYSTEM_LOG_H_ */
