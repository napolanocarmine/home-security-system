/*
 * ds1307rtc.c
 *
 *  Created on: May 26, 2020
 *      Author: 2017
 */


#include "ds1307rtc.h"
#include "system.h"
/**
 * @brief Define BUFFER_SIZE constant
 */
#define BUFFER_SIZE (7)

/**
 * @brief Input buffer where write data received by the peripheral
 */
uint8_t input_buffer[BUFFER_SIZE];

/**
 * @brief Define months' days vector
 */
uint8_t months[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};


/**
 * @brief 	RTC initializer
 * @param 	rtc  	pointer to the rtc_t structure to initialize
 * @param 	i2c 	pointer to the I2C peripheral handler to use
 * @return 	the operation result
 */
int8_t ds1307rtc_init(rtc_t *rtc, I2C_HandleTypeDef *i2c){

 	HAL_StatusTypeDef returnValue = HAL_I2C_IsDeviceReady(i2c, DS1307_ADDRESS, MAX_RETRY, HAL_MAX_DELAY); // check if the device is ready

	if(returnValue != HAL_OK)
	{
		return DS1307_ERR;

	}else{

		rtc->i2c = i2c; //assign the i2c peripheral handler

		ds1307rtc_update_date_time(rtc); // call update date_time structure procedure for initialization of the structure's fields

	}

	return DS1307_OK;


}

/**
 * @brief 	Convert the represented decimal input value into the corresponding BCD format
 * @param 	value 	decimal value to be converted
 * @return 	converted value
 */
static uint8_t convert_dec2BCD(uint8_t value){
	return (value/10<<4)+(value%10);

}

/**
 * @brief 	Convert the represented  bcd input value into the corresponding decimal format
 * @param 	bcd_value 	bcd value to be converted
 * @return 	converted value
 */
static uint8_t convert_BCD2dec(uint8_t bcd_value){
	return 10*(bcd_value>>4 & 0x7) + (bcd_value & 0xF);
}

/**
 * @brief 	Prepare the buffer for the writing operation
 * @param 	date_time 	reference to data_structure to be written
 * @param 	buffer 		reference to output buffer
 * @return 	void
 */
static void prepare_out_buffer(date_time_t *date_time, uint8_t *buffer){
	/*Convert all the date_time structure fields into BCD format*/
	buffer[0] = convert_dec2BCD(date_time->seconds);
	buffer[1] = convert_dec2BCD(date_time->minutes);
	buffer[2] = convert_dec2BCD(date_time->hours);
	buffer[3] = convert_dec2BCD(date_time->day);
	buffer[4] = convert_dec2BCD(date_time->date);
	buffer[5] = convert_dec2BCD(date_time->month);
	buffer[6] = convert_dec2BCD(date_time->year);
}

/**
 * @brief 	Convert the input buffer and update the date_time structure fields with the corresponding values
 * @param 	date_time 	reference to data_structure to update
 * @param 	buffer 		reference to buffer where data are inserted
 * @return	void
 */
static void convert_input_buffer(date_time_t *date_time, uint8_t *buffer){
	/*Convert all the buffer values into decimal format, and assign them to the corresponding date_time structure fields */
	date_time->seconds = convert_BCD2dec(buffer[0]);
	date_time->minutes = convert_BCD2dec(buffer[1]);
	date_time->hours = convert_BCD2dec(buffer[2]);
	date_time->day = convert_BCD2dec(buffer[3]);
	date_time->date = convert_BCD2dec(buffer[4]);
	date_time->month = convert_BCD2dec(buffer[5]);
	date_time->year = convert_BCD2dec(buffer[6]);

}

/**
 * @brief 	Update the date_time structure referenced by the rtc pointer in non-blocking mode
 * @param 	rtc 	RTC handler whose date and time must be updated
 * @return 	operation result
 */
int8_t ds1307rtc_update_date_time_DMA(rtc_t *rtc){

	// request for memory reading
	HAL_StatusTypeDef return_value= HAL_I2C_Mem_Read_DMA(rtc->i2c, DS1307_ADDRESS, DS1307_SECONDS, ADDRESS_SIZE, input_buffer, BUFFER_SIZE);

	if(return_value != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	return DS1307_OK;

}



/**
* @brief 	Write the rtc's date_time structure values into peripheral memory, in non-blocking mode.
* @param 	rtc 	pointer to the rtc handler to use
* @return 	operation result
*/
int8_t ds1307rtc_set_date_time_DMA(rtc_t *rtc){

	uint8_t out_buffer[BUFFER_SIZE]; // define the output buffer

	prepare_out_buffer(&(rtc->date_time), out_buffer); // call the procedure for preparing the output buffer

	// request for writing data
	HAL_StatusTypeDef returnValue = HAL_I2C_Mem_Write_DMA(rtc->i2c, DS1307_ADDRESS, DS1307_SECONDS, ADDRESS_SIZE, out_buffer, BUFFER_SIZE);

	if(returnValue != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	return DS1307_OK;
}

/**
 * @brief 	Update the date_time structure referenced by the rtc pointer in blocking mode
 * @param 	rtc 	RTC handler whose date and time must be updated
 * @return 	operation result
 */
int8_t ds1307rtc_update_date_time(rtc_t *rtc){

	HAL_StatusTypeDef return_value;

	// request for memory reading
	return_value = HAL_I2C_Mem_Read(rtc->i2c, DS1307_ADDRESS, DS1307_SECONDS, ADDRESS_SIZE, input_buffer, BUFFER_SIZE,HAL_MAX_DELAY);

	if(return_value != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	convert_input_buffer(&(rtc->date_time), input_buffer);
	return DS1307_OK;

}
/**
* @brief 	Write the rtc's date_time structure values into peripheral memory, in non-blocking mode.
* @param 	rtc 	pointer to the rtc handler to use
* @return 	operation result
*/
int8_t ds1307rtc_set_date_time(rtc_t *rtc){

	uint8_t out_buffer[BUFFER_SIZE];

	prepare_out_buffer(&(rtc->date_time), out_buffer); // call the procedure for preparing the output buffer

	// request for writing data
	HAL_StatusTypeDef returnValue = HAL_I2C_Mem_Write(rtc->i2c, DS1307_ADDRESS, DS1307_SECONDS, ADDRESS_SIZE, out_buffer, BUFFER_SIZE,HAL_MAX_DELAY);

	if(returnValue != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	return DS1307_OK;

}

/**
 * @brief 	Set the internal rtc date structure fields;
 * @param 	rtc 	pointer to the rtc handler to use
 * @param 	year 	year value to set
 * @param 	month 	month value to set
 * @param 	date 	date value to set
 * @return 	operation result
 */
int8_t set_date(rtc_t *rtc,int8_t year,int8_t month,int8_t date){

	if((year>=0 && year<=99) && (month>=1 && month<=12) && (date>=1 && date<=31)){
		// check if the input parameters have a suitable value
		if(date <= months[month-1]){

			rtc->date_time.year = year;
			rtc->date_time.month = month;
			rtc->date_time.date = date;
			return DS1307_OK;

		}

	}

	return DS1307_ERR;
}

/**
 * @brief 	Set the internal rtc time structure fields;
 * @param 	rtc 	pointer to the rtc handler to use
 * @param 	hour 	hour value to set
 * @param 	minute 	minute value to set
 * @param 	second 	second value to set
 * @return 	operation result
 */
int8_t set_time(rtc_t *rtc,int8_t hour,int8_t minute,int8_t second){

	if((hour>=0 && hour<=23) && (minute>=0 && minute<=59) && (second>=0 && second<=59)){
		// check if the input parameters have a suitable value
		rtc->date_time.hours = hour;
		rtc->date_time.minutes = minute;
		rtc->date_time.seconds = second;

		return DS1307_OK;

	}

	return DS1307_ERR;
}

/**
 * @brief 	Get the year value stored by the rtc date_time structure;
 * @param 	rtc 	pointer to the rtc handler to use
 * @return 	year value
 */
uint8_t get_year(rtc_t *rtc){
	return rtc->date_time.year;
}

/**
 * @brief get the month value stored by the rtc date_time structure;
 * @param rtc pointer to the rtc handler to use
 * @return month value
 */
uint8_t get_month(rtc_t *rtc){
	return rtc->date_time.month;
}

/**
 * @brief 	Get the date value stored by the rtc date_time structure;
 * @param 	rtc 	pointer to the rtc handler to use
 * @return 	date value
 */
uint8_t get_date(rtc_t *rtc){
	return rtc->date_time.date;
}

/**
 * @brief 	Get the hour value stored by the rtc date_time structure;
 * @param 	rtc 	pointer to the rtc handler to use
 * @return 	hour value
 */
uint8_t get_hour(rtc_t *rtc){
	return rtc->date_time.hours;
}

/**
 * @brief 	Get the minute value stored by the rtc date_time structure;
 * @param 	rtc 	pointer to the rtc handler to use
 * @return 	minute value
 */
uint8_t get_minute(rtc_t *rtc){
	return rtc->date_time.minutes;
}

/**
 * @brief 	Get the second value stored by the rtc date_time structure;
 * @param 	rtc 	pointer to the rtc handler to use
 * @return	second value
 */
uint8_t get_second(rtc_t *rtc){
	return rtc->date_time.seconds;
}

/**
 * @brief 	I2C Memory Rx transfer completed callback redefinition
 * @param 	hi2c 	pointer to the peripheral handler that has raised the interrupt
 * @return 	void
 */
void HAL_I2C_MemRxCpltCallback (I2C_HandleTypeDef *hi2c){

	convert_input_buffer(&(system.rtc->date_time), input_buffer); // update system rtc date_time structure

	log_callback_tx(); // call the system_log callback procedure
}
