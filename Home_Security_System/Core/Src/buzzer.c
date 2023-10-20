/*
 * buzzer.c
 *
 *  Created on: May 26, 2020
 *      Author: emiso
 */


#include "buzzer.h"
#include "stm32f4xx_hal.h"
#include "system.h"

/**
 * @brief  Initialize buzzer
 * @param  buzzer	pointer to buzzer_t structure
 * @param  state	buzzer state, it can assume the following value:
 * 						-		BUZZER_ACTIVE,
 *						-		BUZZER_INACTIVE
 * @param  timer	pointer to type TIM_HandleTypeDef
 * @note   The assigned timer is the TIM3 in PWM mode
 */
void init_buzzer(buzzer_t *buzzer, buzzer_state_t state, TIM_HandleTypeDef *timer){

	buzzer->state = state;
	buzzer->timer = timer;
}

/**
 * @brief  Activate buzzer with a specific pulse
 * @param  buzzer	pointer to buzzer_t structure
 * @param  pulse	integer value used to set pulse in PWM timer
 * @note   Different pulses are used to set different ringtones
 */
void activate_buzzer(buzzer_t *buzzer, int pulse){

	buzzer->state = BUZZER_ACTIVE;
	__HAL_TIM_SET_COMPARE(buzzer->timer, TIM_CHANNEL_1, pulse);
	reset_timer_counter(buzzer->timer);
	HAL_TIM_PWM_Start_IT(buzzer->timer, TIM_CHANNEL_1);
}

/**
 * @brief  Deactivate buzzer
 * @param  buzzer	pointer to buzzer_t structure
 *
 */
void deactivate_buzzer(buzzer_t *buzzer){

	buzzer->state = BUZZER_INACTIVE;

	reset_timer_counter(buzzer->timer);
	HAL_TIM_PWM_Stop_IT(buzzer->timer, TIM_CHANNEL_1);

}

/**
 * @brief   Get buzzer state
 * @param   buzzer	 pointer to buzzer_t structure
 * @retval  buzzer state
 */
int8_t get_state_buzzer(buzzer_t *buzzer){

	return buzzer->state;

}

/**
 * @brief  Manage the Pulse Finished Callback
 * @param  htim	pointer to type TIM_HandleTypeDef
 * @note   It is used to deactivate the buzzer after one pulse when a command is accepted
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef  *htim){

	if(htim->Instance == TIM3 && __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1) == COMMAND_PULSE){
		deactivate_buzzer(system.buzzer);
	}
}

