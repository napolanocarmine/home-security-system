/*
 * buzzer.h
 *
 *  Created on: May 26, 2020
 *      Author: emiso
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "stm32f4xx_hal.h"

/**
 * Define buzzer ringtone
 */
typedef enum{
	RINGTONE_PIR,
	RINGTONE_BARRIER,
	RINGTONE_BOTH
}ringtone_t;

/**
 * Define buzzer status
 */
typedef enum{
	BUZZER_ACTIVE,
	BUZZER_INACTIVE
}buzzer_state_t;

/**
 * Define buzzer struct
 */
typedef struct{
	buzzer_state_t state;
	ringtone_t ringtone;
	TIM_HandleTypeDef *timer;
}buzzer_t;


/**
 * Initialize buzzer
 */
void init_buzzer(buzzer_t *buzzer, buzzer_state_t state, TIM_HandleTypeDef *timer);
/**
 * It active buzzer with a specific ringtone
 */
void activate_buzzer(buzzer_t *buzzer, int period);

/*
 * Get state buzzer state
 */
int8_t get_state_buzzer(buzzer_t *buzzer);

/**
 * Deactivate buzzer
 */
void deactivate_buzzer(buzzer_t *buzzer);


#endif /* INC_BUZZER_H_ */
