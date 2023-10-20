/*
 * system.h
 *
 *  Created on: May 26, 2020
 *      Author: emiso
 */

#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_

#include <module_barrier.h>
#include "module_pir.h"
#include "buzzer.h"
#include "tim.h"
#include "uart_handler.h"
#include "ds1307rtc.h"
#include "system_log.h"
#include "configuration_protocol.h"

#define ACTIVE_AREA_ALLARM ("A#")
#define ACTIVE_BARRIER_ALLARM ("B#")
#define ACTIVE_BOTH_ALLARM ("C#")
#define ACTIVE_SYSTEM ("D#")

#define DISABLE_AREA_ALLARM ("A*")
#define DISABLE_BARRIER_ALLARM ("B*")
#define DISABLE_BOTH_ALLARM ("C*")
#define DISABLE_SYSTEM ("D*")

#define COMMAND_SIZE (2)

#define COMMAND_EXECUTED (2)
#define USER_PIN_OK (1)
#define COMMAND_ACCEPTED (0)
#define SYS_OK (0)
#define SYS_ERR (-1)
#define WRONG_USER_PIN (-1)
#define COMMAND_REJECTED (-2)
#define COMMAND_ERROR (-3)
#define COMMAND_BUFFER_SIZE (7)

#define COMMAND_PULSE (99)
#define PIR_PULSE (199)
#define BARRIER_PULSE (499)
#define BOTH_PULSE (999)

/**
 * Define system status
 */
typedef enum{

	SYSTEM_ACTIVE,
	SYSTEM_INACTIVE,
	SYSTEM_ALARMED,

}system_state_t;

/**
 * Define system struct
 */
typedef struct{

	system_state_t state;
	module_barrier_t *barrier;
	module_pir_t *pir;
	buzzer_t *buzzer;

	system_configuration_t *system_configuration;
	configuration_protocol_t *protocol;
	system_log_t *system_log;
	rtc_t *rtc;

} system_t;

extern system_t system;

/**
 * Initialize the system
 */
int8_t init_system();

/**
 * Initialize all the support elements
 */

int8_t init_elements();

/**
 * Run the system
 */

void run_system();

/**
 * Inizialize all the sensor
 */
void init_sensor(module_pir_t *pir, digital_sensor_t* sensor_pir,module_barrier_t *barrier, laser_t *laser, photoresistor_t *photorestor, buzzer_t *buzzer);

/**
 * Active the system
 */
int8_t activate_system(system_t *system);

/**
 * Deactive the system
 */
int8_t deactivate_system(system_t *system);

/**
 * Alarm the system
 */
void alarm_system(system_t *system, uint16_t pulse);

/**
 * Dealarm the system
 */
void dealarm_system(system_t *system);
/**
 * Active the barrier sensor
 */
int8_t activate_module_barrier(system_t *system);

/**
 * Deactive the barrier sensor
 */
int8_t deactivate_module_barrier(system_t *system);

/**
 * Active the pir sensor
 */
int8_t activate_module_pir(system_t *system);

/**
 * Deactive the pir sensor
 */
int8_t deactivate_module_pir(system_t *system);

/**
 * Deactive both the module
 */
int8_t deactivate_both(system_t *system);

/**
 * Used from the pir callback to communicate its alarmed state
 */
void alarm_pir(module_pir_t *pir);

/**
 * Used from the barrier callback to communicate its alarmed state
 */
void alarm_barrier(module_barrier_t *barrier);

/**
 * Get the pin inserted by user
 */
void get_user_pin(uint8_t *dest);

/**
 * Execute the command inserted by user
 */
int8_t execute_command(uint8_t* command);


#endif /* INC_SYSTEM_H_ */
