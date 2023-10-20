/*
 * system_log->c
 *
 *  Created on: 30 May 2020
 *      Author: 2017
 */


#include <timer_handler.h>
#include "system_log.h"
#include "system.h"
#include "ds1307rtc.h"
#include "uart_handler.h"
#include "string.h"
#include "module_barrier.h"
#include "module_pir.h"
#include "stm32f4xx_hal_uart.h"
#include "stdio.h"

#define RTC_COMUNICATION_PROBLEM ("RTC PROBLEM: CHECK CONNECTIONS AND RESTART THE BOARD\n\r")

/**
 * @brief buffer where insert system log message
 */
char msg[100];

/**
 * @brief Output date time buffer size
 */
#define DATE_TIME_OUTPUT_BUFFER_SIZE (21)

/**
 * @brief output date_time buffer
 */
uint8_t output_date_time_buffer[DATE_TIME_OUTPUT_BUFFER_SIZE];

/**
 * @brief Char variable for dash char
 */
uint8_t dash = '-';

/**
 * @brief Buffer to carriage return
 */
char* carriage_return = "\n\r";

/**
 * @brief  Initialize the system log
 * @param  system_log 		pointer to system log structure
 * @param  rtc				pointer to rtc structure
 * @param  uart_handler		pointer to uart handler structure
 * @param  timer			pointer to timer
 */
void init_system_log(system_log_t *system_log, rtc_t *rtc, uart_handler_t *uart_handler, TIM_HandleTypeDef *timer){

	system_log->state = IDLE_L;
	system_log->rtc = rtc;
	system_log->uart = uart_handler;
	system_log->timer = timer;

}

/**
 * @brief  Start system log protocol
 * @param  system_log	pointer to system log structure
 * @note   Start the protocol starting the timer that counts each period
 */
void start_system_log(system_log_t *system_log){

	system_log->state = START_L;

	set_timer_period(system_log->timer, SYSTEM_LOG_PERIOD);
	start_timer_IT(system_log->timer);

}

/**
 * @brief  Stop system log protocol
 * @param  system_log	pointer to system log structure
 * @note   Stop system log protocol stopping the related timer
 */
void stop_system_log(system_log_t *system_log){

	system_log->state = STOP_L;
	stop_timer_IT(system_log->timer);

}

/**
 * @brief  Start the procedure to send the new system log message
 * @param  system_log	pointer to system_log structure
 * @note   called by TIM10 ElapsedPeriod_Callback.
 * 		   Perform a rtc update request
 */
void start_send_log_message(system_log_t *system_log){

	if( ds1307rtc_update_date_time_DMA(system_log->rtc) != DS1307_OK)
		system_log_send_message(system_log, (uint8_t *)RTC_COMUNICATION_PROBLEM, strlen(RTC_COMUNICATION_PROBLEM));
}

/**
 * @brief   Get the sensor state and convert it into  string
 * @param   state	 sensor state
 * @retval  return the correspondent string
 */
char* get_state_string(module_state_t state){

	if(state == SENSOR_ACTIVE){
		return ACTIVE_STRING;
	}else if(state == SENSOR_INACTIVE){
		return INACTIVE_STRING;
	}

	return ALLARMED_STRING;
}

/**
 * @brief   Send the giver buffer over uart in interrupt mode
 * @param   system_log		pointer to system_log structure
 * @param	buffer			pointer to message buffer
 * @param	buffer_size		buffer size
 * @retval  operation result
 */
int8_t system_log_send_message_IT(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size){

	return uart_handler_send_message_IT(system_log->uart, buffer, buffer_size);

}

/**
 * @brief   Send the giver buffer over uart in DMA mode
 * @param   system_log		pointer to system_log structure
 * @param	buffer			pointer to message buffer
 * @param	buffer_size		buffer size
 * @retval  operation result
 */
int8_t system_log_send_message_DMA(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size){

	return uart_handler_send_message_DMA(system_log->uart, buffer, buffer_size);

}


/**
 * @brief   Send the given buffer over uart, in blocking mode
 * @param   system_log		pointer to system_log structure
 * @param	buffer			pointer to message buffer
 * @param	buffer_size		buffer size
 * @retval  operation result
 */
int8_t system_log_send_message(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size){

	return uart_handler_send_message(system_log->uart, buffer, buffer_size);

}

/**
 * @brief  	Receive data from uart and insert them in the given buffer, in interrupt mode
 * @param   system_log		pointer to system_log structure
 * @param	buffer			pointer to buffer where insert received data
 * @param	buffer_size		buffer size
 * @retval  operation result
 */
int8_t system_log_receive_message_IT(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size){

	return uart_handler_receive_message_IT(system_log->uart, buffer, buffer_size);
}


/**
 * @brief  	Receive data from UART and insert them in the given buffer, in DMA mode
 * @param   system_log		pointer to system_log structure
 * @param	buffer			pointer to buffer where insert received data
 * @param	buffer_size		buffer size
 * @retval  operation result
 */
int8_t system_log_receive_message_DMA(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size){

	return uart_handler_receive_message_DMA(system_log->uart, buffer, buffer_size);
}



/**
 * @brief   Receive data from uart and insert them in the given buffer, in blocking mode
 * @param   system_log		pointer to system_log structure
 * @param	buffer			pointer to buffer where insert received data
 * @param	buffer_size		buffer size
 * @retval  operation result
 */
int8_t system_log_receive_message(system_log_t *system_log, uint8_t *buffer, int16_t buffer_size){

	return uart_handler_receive_message_IT(system_log->uart, buffer, buffer_size);
}

/**
 * @brief	Convert the given decimal value to the corresponding ASCII decimal representation
 * @param	value	decimal value
 * @param	buffer	pointer to an uint8_t array where stored the ASCII decimal representation
 */
void convert_uint8_to_output_buffer(uint8_t value, uint8_t *buffer){

	buffer[0] = value/10 + '0';
	buffer[1] = value%10 + '0' ;

}

/**
 * @brief Format the output_date_time_buffer in order to be sent over uart.
 */
void prepare_date_time_buffer(){

	output_date_time_buffer[0] = '[';
	convert_uint8_to_output_buffer(get_date(system.rtc), output_date_time_buffer+1);
	output_date_time_buffer[3] = '-';
	convert_uint8_to_output_buffer(get_month(system.rtc), output_date_time_buffer+4);
	output_date_time_buffer[6] = '-';
	output_date_time_buffer[7] = '2';
	output_date_time_buffer[8] = '0';
	convert_uint8_to_output_buffer(get_year(system.rtc), output_date_time_buffer+9);
	output_date_time_buffer[11] = ' ';
	convert_uint8_to_output_buffer(get_hour(system.rtc), output_date_time_buffer+12);
	output_date_time_buffer[14] = ':';
	convert_uint8_to_output_buffer(get_minute(system.rtc), output_date_time_buffer+15);
	output_date_time_buffer[17] = ':';
	convert_uint8_to_output_buffer(get_second(system.rtc), output_date_time_buffer+18);
	output_date_time_buffer[20] = ']';

}


/**
 * @brief	Implement the system log procedure
 * @note	Based on the system log state. It takes the buffer corresponding to the state and then transmit it over UART
 * 			It transmits, following this order:
 * 				- Date & Time
 * 				- Sensors name and state
 */
void log_callback_tx(){

	if(system.system_log->state == START_L){

		prepare_date_time_buffer(); // format output_date_time_buffer
		sprintf(msg, "%s AREA %s - BARRIER %s \n\r",(char *)output_date_time_buffer, get_state_string(get_state_pir(system.pir)), get_state_string(get_state_barrier(system.barrier)));
		system.system_log->state = SYSTEM_STATE_T; // set the DATE_TIME_T state
		system_log_send_message_DMA(system.system_log, (uint8_t *)msg, strlen(msg));// send the system log message

	}
	else if((system.system_log->state == SYSTEM_STATE_T)){
		system.system_log->state = START_L;
	}

}
