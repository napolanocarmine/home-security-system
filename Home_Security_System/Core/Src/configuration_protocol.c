#include "configuration_protocol.h"
#include "usart.h"
#include "tim.h"
#include <string.h>
#include <timer_handler.h>
#include "ds1307rtc.h"
#include "system_log.h"
#include "system.h"
#include "stdio.h"
/**
 * @brief Define timer's period for configuration task.
 */
#define CONFIGURATION_PERIOD (29999)

/**
 * @brief buffer for sending the inserted date and time back
 */
char msg[4];

/**
 * @brief Function that checks the configuration parameters inserted by the user
 */
static int8_t check_configuration_fields(configuration_protocol_t *protocol);

/**
 * @brief Function that loads the parameters inserted by the user into the system_configuration structure
 */
static int8_t load_custom_configuration(configuration_protocol_t *protocol);

/**
 * @brief Function that loads the default configuration parameters into the system_configuration structure
 */
static int8_t load_default_configuration(configuration_protocol_t *protocol);


/**
 * Define global buffer variables.
 */

/**
 * @brief User PIN buffer
 */
uint8_t pin_buffer[PIN_SIZE];

/**
 * @brief Delay of the first sensor
 */
uint8_t delay1_buffer[DELAY_SIZE];

/**
 * @brief Delay of the second sensor
 */
uint8_t delay2_buffer[DELAY_SIZE];

/**
 * @brief Duration of the alarm
 */
uint8_t duration_buffer[DURATION_SIZE];

/**
 * @brief Buffer for date and time
 */
uint8_t date_time_buffer[DATE_TIME_BUFFER_SIZE];


/**
 * @brief Default USER PIN
 */
user_pin DEFAULT_USER_PIN[PIN_SIZE] = {'0','0','0','0'};

/**
 * @brief  Initialize protocol structure
 * @param  protocol			pointer to configuration protocol structure
 * @param  configuration	pointer to system configuration structure
 * @param  timer			pointer to timer to use in the configuration procedure
 * @param  rtc				pointer to rtc structure
 * @return void
 * @note   It is used to initialize the configuration protocol. It assigns:
 * 				-   system configuration,
 * 				-   rtc
 * 				-   state
 * 				-   timer
 */
void init_protocol(configuration_protocol_t *protocol, system_configuration_t *configuration, TIM_HandleTypeDef *timer, rtc_t *rtc){

	protocol->configuration = configuration;

	protocol->rtc = rtc;

	protocol->state = IDLE_P; // set the state to IDLE

	protocol->timer = timer;

	set_timer_period(protocol->timer, CONFIGURATION_PERIOD); // configure the timer's period

}

/**
 * @brief   Function that implements the configuration protocol procedure in blocking-mode
 * @param   protocol  	 pointer to the protocol structure to use
 * @return  the final protocol' state
 */
protocol_status_type configuration_protocol(configuration_protocol_t *protocol){

	protocol->state = START_P; // set the state to start

	system_log_send_message_IT(system.system_log, (uint8_t *)SYSTEM_BOOT, strlen(SYSTEM_BOOT)); // send the SYSTEM BOOT message over uart

	start_timer_IT(protocol->timer); // start the protocol timer

	while(protocol->state != END_CUSTOM && protocol->state != END_DEFAULT){} // wait until the protocol is finished

	if(protocol->state == END_DEFAULT){
		// check if the protocol is finished due to the timer period elapsed
		if(load_default_configuration(protocol) == PROTOCOL_OK){ // load the default configuration
			system_log_send_message(system.system_log, (uint8_t *)DEFAULT_CONFIGURATION_LOADED, strlen(DEFAULT_CONFIGURATION_LOADED)); // send "SYSTEM CONFIGURATION REJECTED" MESSAGE
			return protocol->state;
		}
	}else{
		// the user has compiled all the configuration field
		if(check_configuration_fields(protocol) == PROTOCOL_OK){ // check if the parameters inserted by the user are good

			if(load_custom_configuration(protocol) == PROTOCOL_OK){ // load the configuration inserted by the user
				// send "System Configuration Loaded" message
				system_log_send_message(system.system_log, (uint8_t *)CUSTOM_CONFIGURATION_LOADED, strlen(CUSTOM_CONFIGURATION_LOADED));
				return protocol->state;
			}

		}else{
				// load the custom configuration if the parameters inserted by the user are not good
				if(load_default_configuration(protocol)==PROTOCOL_OK){
					// send "System Configuration Rejected" message
					protocol->state = END_DEFAULT;
					system_log_send_message(system.system_log, (uint8_t *)DEFAULT_CONFIGURATION_LOADED, strlen(DEFAULT_CONFIGURATION_LOADED));
					return protocol->state;
				}
			}
	}

	// send an error message if the loading procedure was not successfully
	system_log_send_message(system.system_log, (uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
	protocol->state = END_ERR;
	return protocol->state;

}

/**
 * @brief   Convert input buffer to the corresponding decimal value
 * @param   buffer 	 input buffer
 * @return  the converted value
 */
static uint8_t convert_buffer_to_uint8(uint8_t *buffer){

	return (10*(buffer[0]-'0')) + (buffer[1] - '0');

}

/**
 * @brief   Check if the inserted pin is valid
 * @param   pin 	pointer to the pin buffer
 * @return  operation result
 */
static int8_t check_pin(uint8_t *pin){

	uint8_t i = 0;

	for(i=0; i<PIN_SIZE; i++){

		if(pin[i] < '0' || pin[i] > '9'){
			return PROTOCOL_ERR;
		}

	}

	return PROTOCOL_OK;
}

/**
 * @brief   Check if the inserted delay is valid
 * @param   delay 	 pointer to the delay buffer
 * @return  operation result
 */
static int8_t check_delay(uint8_t *delay){

	uint8_t i = 0;

	for(i=0; i<DELAY_SIZE; i++){

		if((i == 0 && (delay[i] < '0' || delay[i] > '3'))
			|| (i == 1 && (delay[i] < '0' || (delay[i] > '9')))){

			return PROTOCOL_ERR;

		}else if( (i == 1 && (delay[i] > '0' && delay[0] == '3' ))){

			return PROTOCOL_ERR;

		}

	}

	return PROTOCOL_OK;

}

/**
 * @brief   Check if the inserted duration is valid
 * @param   duration	 pointer to the duration buffer
 * @return  operation result
 */
static int8_t check_duration(uint8_t *duration){

	uint8_t i = 0;

	for(i=0; i<DURATION_SIZE; i++){

		if((i == 0 && (duration[i] < '0' || duration[i] > '6'))
			|| (i == 1 && (duration[i] < '0' || (duration[i] > '9')))){

			return PROTOCOL_ERR;

		}else if( (i == 1 && ((duration[i] > '0' && duration[0] == '6' ) ||  (duration[i] < '5' && duration[0] == '0' )))){

			return PROTOCOL_ERR;

		}

	}

	return PROTOCOL_OK;

}

/**
 * @brief   Take the date and time parameters inserted by the user from date_time buffer
 * @param   protocol 	pointer to the protocol structure
 * @param   date_time 	pointer to the date and time buffer
 * @return  operation result
 */
static int8_t take_date_time_from_buffer(configuration_protocol_t *protocol, uint8_t *date_time){

	int8_t date = convert_buffer_to_uint8(date_time); // convert date from characters to decimal
	int8_t month = convert_buffer_to_uint8(date_time+DATE_TIME_SIZE);// convert month from characters to decimal
	int8_t year = convert_buffer_to_uint8(date_time+2*DATE_TIME_SIZE);// convert year from characters to decimal
	int8_t hour = convert_buffer_to_uint8(date_time+3*DATE_TIME_SIZE);// convert hour from characters to decimal
	int8_t minute = convert_buffer_to_uint8(date_time+4*DATE_TIME_SIZE);// convert minute from characters to decimal
	int8_t second = convert_buffer_to_uint8(date_time+5*DATE_TIME_SIZE);// convert second from characters to decimal

	// update the rtc date_time structure
	if(set_date(protocol->rtc,year,month,date) == DS1307_OK && set_time(protocol->rtc, hour, minute, second) == DS1307_OK){
		return PROTOCOL_OK;
	}

	return PROTOCOL_ERR;
}

/**
 * @brief   Check if the date and time parameters inserted by the user are good
 * @param   protocol 	pointer to the protocol structure
 * @param   date_time 	pointer to the date and time buffer
 * @return  operation result
 */
static int8_t check_date_time(configuration_protocol_t *protocol, uint8_t *date_time){

	int i=0;

	for(i=0; i<DATE_TIME_BUFFER_SIZE; i++){

		if(date_time[i]<'0' || date_time[i] > '9'){
			return PROTOCOL_ERR;
		}

	}

	return take_date_time_from_buffer(protocol, date_time);
}

/**
 * @brief   Check configuration parameters inserted by the user
 * @param   protocol 	pointer to the protocol structure
 * @return  operation result
 */
static int8_t check_configuration_fields(configuration_protocol_t *protocol){

	// call the procedure for checking the buffers
	if( check_pin(pin_buffer) != PROTOCOL_OK || check_delay(delay1_buffer) != PROTOCOL_OK
		|| check_delay(delay2_buffer) != PROTOCOL_OK || check_duration(duration_buffer) != PROTOCOL_OK || check_date_time(protocol, date_time_buffer) != PROTOCOL_OK){

		return PROTOCOL_ERR;
	}

	return PROTOCOL_OK;
}

/**
 * @brief   Load the custom configuration
 * @param 	protocol 	pointer to the protocol structure
 * @return 	operation result
 */
static int8_t load_custom_configuration(configuration_protocol_t *protocol){

	memcpy(protocol->configuration->pin, pin_buffer, PIN_SIZE); // set the pin field to the inserted pin

	protocol->configuration->sensor_delay_1 = convert_buffer_to_uint8(delay1_buffer); // set the sensor delay field to the inserted delay

	protocol->configuration->sensor_delay_2 = convert_buffer_to_uint8(delay2_buffer); // set the sensor delay field to the inserted delay

	protocol->configuration->duration =  convert_buffer_to_uint8(duration_buffer); // set the duration field to the inserted duration

	if (ds1307rtc_set_date_time(protocol->rtc) == DS1307_OK){ // update the device memory with the inserted values
		return PROTOCOL_OK;
	}else{
		return PROTOCOL_ERR;
	}
}

/**
 * @brief 	Load the default configuration
 * @param 	protocol 	pointer to the protocol structure
 * @return 	operation result
 */
static int8_t load_default_configuration(configuration_protocol_t *protocol){

	memcpy(protocol->configuration->pin, DEFAULT_USER_PIN, PIN_SIZE); // load the default pin
	protocol->configuration->sensor_delay_1 = DEFAULT_DELAY; // load the default sensor delay
	protocol->configuration->sensor_delay_2 = DEFAULT_DELAY; // load the default sensor delay
	protocol->configuration->duration = DEFAULT_DURATION; // load the default duration time

	if (ds1307rtc_update_date_time(protocol->rtc) == DS1307_OK){
		return PROTOCOL_OK;
	}else{
		return PROTOCOL_ERR;
	}

}

/**
 * @brief 	Function that implements the tx callback procedure
 * @return 	void
 */
void protocol_callback_tx(){

	if(system.protocol->state == START_P){

			system.protocol->state = PIN_R;
			system_log_send_message_IT(system.system_log, (uint8_t *)INSERT_PIN , strlen(INSERT_PIN));
			system_log_receive_message_IT(system.system_log, pin_buffer, PIN_SIZE);// wait for receiving pin

		}else if(system.protocol->state  == PIN_T){

			system.protocol->state = DELAY1_R; // wait for delay1
			system_log_send_message_IT(system.system_log, (uint8_t *)INSERT_DELAY_SENSOR_1,strlen(INSERT_DELAY_SENSOR_1));
			system_log_receive_message_IT(system.system_log, delay1_buffer, DELAY_SIZE); // wait for receiving the first delay

		}else if(system.protocol->state == DELAY1_T){

			system.protocol->state = DELAY2_R;
			system_log_send_message_IT(system.system_log, (uint8_t *)INSERT_DELAY_SENSOR_2,strlen(INSERT_DELAY_SENSOR_2));
			system_log_receive_message_IT(system.system_log, delay2_buffer, DELAY_SIZE);// wait for receiving the second delay

		}else if(system.protocol->state == DELAY2_T){

			system.protocol->state = DURATION_R; // wait for duration
			system_log_send_message_IT(system.system_log, (uint8_t *)INSERT_ALLARM_DURATION,strlen(INSERT_ALLARM_DURATION));
			system_log_receive_message_IT(system.system_log, duration_buffer, DURATION_SIZE);// wait for receiving the duration

		}else if(system.protocol->state == DURATION_T){

			system.protocol->state = DATE_R;
			system_log_send_message_IT(system.system_log, (uint8_t *)INSERT_DATE_TIME,strlen(INSERT_DATE_TIME));
			system_log_receive_message_IT(system.system_log, date_time_buffer, DATE_TIME_SIZE);// wait for receiving the  date


		}else if(system.protocol->state == DATE_T){

			system.protocol->state = MONTH_R;
			system_log_receive_message_IT(system.system_log, date_time_buffer+DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving month

		}
		else if(system.protocol->state == MONTH_T){

			system.protocol->state = YEAR_R;
			system_log_receive_message_IT(system.system_log, date_time_buffer+2*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving year

		}
		else if(system.protocol->state == YEAR_T){

			system.protocol->state = HOUR_R;
			system_log_receive_message_IT(system.system_log, date_time_buffer+3*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving hour

		}
		else if(system.protocol->state == HOUR_T){

			system.protocol->state = MINUTE_R;
			system_log_receive_message_IT(system.system_log, date_time_buffer+4*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving minute

		}
		else if(system.protocol->state == MINUTE_T){

			system.protocol->state = SECOND_R;
			system_log_receive_message_IT(system.system_log, date_time_buffer+5*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving second

		}
		else if(system.protocol->state == SECOND_T){

			stop_timer_IT(system.protocol->timer);
			reset_timer_counter(system.protocol->timer);
			system.protocol->state = END_CUSTOM;

		}

}

/**
 * @brief 	Function that implements the rx callback procedure
 * @return 	void
 */
void protocol_callback_rx(){

	if(system.protocol->state == PIN_R ){

		system.protocol->state = PIN_T;
		system_log_send_message_IT(system.system_log, pin_buffer, PIN_SIZE); // send the inserted pin

	}else if(system.protocol->state == DELAY1_R){

		system.protocol->state = DELAY1_T;
		system_log_send_message_IT(system.system_log, delay1_buffer,DELAY_SIZE); // send the inserted delay

	}else if(system.protocol->state == DELAY2_R){

		system.protocol->state = DELAY2_T;
		system_log_send_message_IT(system.system_log, delay2_buffer,DELAY_SIZE); // send the inserted delay

	}else if(system.protocol->state == DURATION_R){

		system.protocol->state = DURATION_T;
		system_log_send_message_IT(system.system_log, duration_buffer,DURATION_SIZE); // send the inserted duration

	}
	else if(system.protocol->state == DATE_R){

		system.protocol->state = DATE_T;
		sprintf(msg," %c%c/", (char)date_time_buffer[0], (char)date_time_buffer[1]);
		system_log_send_message_IT(system.system_log, (uint8_t *)msg,strlen(msg)); // send the inserted date

	}
	else if(system.protocol->state == MONTH_R){

		system.protocol->state = MONTH_T;
		sprintf(msg,"%c%c/", (char)date_time_buffer[2], (char)date_time_buffer[3]);
		system_log_send_message_IT(system.system_log, (uint8_t *)msg,strlen(msg)); // send the inserted month

	}
	else if(system.protocol->state == YEAR_R){

		system.protocol->state = YEAR_T;
		sprintf(msg,"%c%c ", (char)date_time_buffer[4], (char)date_time_buffer[5]);
		system_log_send_message_IT(system.system_log, (uint8_t *)msg,strlen(msg)); // send the inserted year

	}
	else if(system.protocol->state == HOUR_R){

		system.protocol->state = HOUR_T;
		sprintf(msg,"%c%c:", (char)date_time_buffer[6], (char)date_time_buffer[7]);
		system_log_send_message_IT(system.system_log, (uint8_t *)msg,strlen(msg)); // send the inserted hour

	}
	else if(system.protocol->state == MINUTE_R){

		system.protocol->state = MINUTE_T;
		sprintf(msg,"%c%c:", (char)date_time_buffer[8], (char)date_time_buffer[9]);
		system_log_send_message_IT(system.system_log, (uint8_t *)msg,strlen(msg)); // send the inserted minute

	}
	else if(system.protocol->state == SECOND_R){

		system.protocol->state = SECOND_T;
		sprintf(msg,"%c%c", (char)date_time_buffer[10], (char)date_time_buffer[11]);
		system_log_send_message_IT(system.system_log, (uint8_t *)msg,strlen(msg)); // send the inserted second

	}

}
