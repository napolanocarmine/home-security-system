/*
 * sensor_barrier.h
 *
 *  Created on: May 26, 2020
 *      Author: emiso
 */

#ifndef INC_MODULE_BARRIER_H_
#define INC_MODULE_BARRIER_H_

#include "sensor.h"
#include "stdint.h"
#include "photoresistor.h"
#include "laser.h"

/**
 * Define barrier struct
 */
typedef struct{

	module_state_t state;

	uint16_t threshold_down;

	uint16_t threshold_up;

	uint16_t threshold;

	photoresistor_t *photoresistor;

	laser_t *laser;

	uint8_t delay;

	uint16_t pulse;

	uint32_t stable_signal;

}module_barrier_t;

/**
 * Initialize module barrier
 */
void init_module_barrier(module_barrier_t *module_barrier,module_state_t state, photoresistor_t *photoresistor, laser_t *laser, uint8_t delay, uint16_t pulse, uint32_t stable_signal);

/**
 * Set the up and down threshold
 */
void set_threshold(module_barrier_t *module_barrier);

/**
 * Get barrier state
 */
module_state_t get_state_barrier(module_barrier_t *module_barrier);

/**
 * Set barrier state
 */
void set_state_barrier(module_barrier_t *module_barrier, module_state_t state);

/**
 * Start the barrier sensor to capture brightness variation
 */
void start_barrier_sensor(module_barrier_t *module_barrier);

/**
 * Stop the barrier sensor (deactive it)
 */
void stop_barrier_sensor(module_barrier_t *module_barrier);

#endif /* INC_MODULE_BARRIER_H_ */
