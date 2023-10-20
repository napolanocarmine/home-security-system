/*
 * system_configuration.h
 *
 *  Created on: May 18, 2020
 *      Author: 2017
 */

#ifndef INC_CONFIGURATION_PROTOCOL_H_

#define INC_CONFIGURATION_PROTOCOL_H_

#include <stdint.h>
#include "timer_handler.h"
#include "stm32f4xx.h"
#include "ds1307rtc.h"
/**
 * Define protocol Error
 */
#define PROTOCOL_OK (0)
#define PROTOCOL_ERR (-1)


/**
 * Define default configuration values
 */
#define DEFAULT_DELAY (0)
#define DEFAULT_DURATION (5)

/**
 * Define messages to sent through UART
 */
#define INSERT_PIN ("\n\rINSERT PIN [xxxx]: ")
#define INSERT_DELAY_SENSOR_1 ("\n\rISERT PIR's DELAY [xx]: ")
#define INSERT_DELAY_SENSOR_2 ("\n\rINSERT BARRIER's DELAY [xx]: ")
#define INSERT_ALLARM_DURATION ("\n\rINSERT ALLARM's DURATION [xx]: ")
#define INSERT_DATE_TIME ("\n\rINSERT DATE and TIME [dd/mm/yr hr:mm:ss]: ")


/**
 * Define configuration's fields size
 */
#define PIN_SIZE (4)
#define DELAY_SIZE (2)
#define DURATION_SIZE (2)
#define DATE_TIME_SIZE (2)
#define DATE_TIME_BUFFER_SIZE (12)

/**
 * Defines protocol status type
 */
typedef enum{
	IDLE_P,
	START_P,
	STOP_P,
	PIN_T,
	PIN_R,
	DELAY1_T,
	DELAY1_R,
	DELAY2_T,
	DELAY2_R,
	DURATION_T,
	DURATION_R,
	DATE_T,
	DATE_R,
	MONTH_T,
	MONTH_R,
	YEAR_T,
	YEAR_R,
	HOUR_T,
	HOUR_R,
	MINUTE_T,
	MINUTE_R,
	SECOND_T,
	SECOND_R,
	END_DEFAULT,
	END_CUSTOM,
	END_ERR
} protocol_status_type;


/**
 * Define USER_PIN type
 */
typedef uint8_t user_pin;
/**
 * Define Delay type;
 */
typedef uint8_t sensor_delay;

/**
 * Define Duration type;
 */
typedef uint8_t alarm_duration;

/**
 * Define system configuration structure
 */
struct system_configuration_s{

	user_pin pin[PIN_SIZE];
	sensor_delay sensor_delay_1;
	sensor_delay sensor_delay_2;
	alarm_duration duration;

};

/**
 * Define System configuration type
 */
typedef struct system_configuration_s system_configuration_t;


/**
 * Define protocol data structure
 */
typedef struct{

	system_configuration_t *configuration;

	protocol_status_type state;

	TIM_HandleTypeDef *timer;

	rtc_t *rtc;


}configuration_protocol_t;

/**
 * Initialize protocol's structure with the given parameters
 */
void init_protocol(configuration_protocol_t *protocol, system_configuration_t *configuration, TIM_HandleTypeDef *timer, rtc_t *rtc);

/**
 * Perform system_configuration procedure in a blocking-mode.
 */
protocol_status_type configuration_protocol(configuration_protocol_t *protocol);

/**
 * Perform the transmission protocol steps
 */
void protocol_callback_tx();

/**
 * Perform the receiving protocol steps
 */
void protocol_callback_rx();

#endif /* INC_CONFIGURATION_PROTOCOL_H_ */
