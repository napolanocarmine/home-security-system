
#ifndef INC_DS1307RTC_H_
#define INC_DS1307RTC_H_

#include <stdint.h>
#include "stm32f4xx.h"

/**
 * Define return values
 */
#define DS1307_OK 		(0)
#define DS1307_ERR		(-1)
#define DS1307_IC2_ERR	(-2)

/**
 * Define addresses
 */
#define DS1307_ADDRESS 				(0xD0)
#define MAX_RETRY					(3)

#define DS1307_SECONDS				(0x00)
#define DS1307_MINUTES				(0x01)
#define DS1307_HOURS				(0x02)
#define DS1307_DAY					(0x03)
#define DS1307_DATE					(0x04)
#define DS1307_MONTH				(0x05)
#define DS1307_YEAR					(0x06)
#define DS1307_CONTROL				(0x07)

/* Bits in control register */
#define DS1307_CONTROL_OUT			(7)
#define DS1307_CONTROL_SQWE			(4)
#define DS1307_CONTROL_RS1			(1)
#define DS1307_CONTROL_RS0			(0)

/**
 * Define ADDRESS and DATA SIZE.
 */
#define ADDRESS_SIZE				(1)
#define DATA_SIZE					(1)


/**
 * Define date_time structure and type
 */
struct date_time_s
{
	uint8_t     seconds; // [0-59]
	uint8_t     minutes; // [0-59]
	uint8_t     hours; //[1-12] or [00-23]
	uint8_t     day; // [1-7]
	uint8_t     date; // [1-31]
	uint8_t     month; // [1-12]
	uint8_t    year; //[00-99]
};

typedef struct date_time_s date_time_t;

/**
 * Define rtc_type
 */
struct rtc_s{

	date_time_t date_time;

	I2C_HandleTypeDef* i2c;

};

typedef struct rtc_s rtc_t;

/**
 * Initialize the rtc structure with the given i2c peripheral handler
 */
int8_t ds1307rtc_init(rtc_t *rtc, I2C_HandleTypeDef *i2c);

/**
 * update the date_time structure referenced by the rtc pointer in non-blocking mode
 */
int8_t ds1307rtc_update_date_time_DMA(rtc_t *rtc);

/**
 * write the rtc's date_time value into memory, in non-blocking mode.
 */
int8_t ds1307rtc_set_date_time_DMA(rtc_t *rtc);

/**
 * update the date_time structure referenced by the rtc pointer in blocking mode
 */
int8_t ds1307rtc_update_date_time(rtc_t *rtc);

/**
 * write the rtc's date_time value into memory, in blocking mode.
 */
int8_t ds1307rtc_set_date_time(rtc_t *rtc);

/**
 * set the internal rtc date structure fields;
 */
int8_t set_date(rtc_t *rtc,int8_t year,int8_t month,int8_t date);

/**
 * set the internal rtc time structure fields;
 */
int8_t set_time(rtc_t *rtc,int8_t hour,int8_t minute,int8_t second);


/**
 * get the current year kept in date_time structure
 */
uint8_t get_year(rtc_t *rtc);

/**
 * get the current month kept in date_time structure
 */
uint8_t get_month(rtc_t *rtc);

/**
 * get the current date kept in date_time structure
 */
uint8_t get_date(rtc_t *rtc);

/**
 * get the current hour kept in date_time structure
 */
uint8_t get_hour(rtc_t *rtc);

/**
 * get the current minute kept in date_time structure
 */
uint8_t get_minute(rtc_t *rtc);

/**
 * get the current second kept in date_time structure
 */
uint8_t get_second(rtc_t *rtc);

#endif /* INC_DS1307RTC_H_ */
