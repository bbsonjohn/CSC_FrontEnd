//------------------------------------------------pwm application---------------------------------------------------
#ifndef _PWM_APP_C_
#define _PWM_APP_C_

#include <appconst.h>
#include <fram_app.c>

//--------------------------------------------function prototypes---------------------------------------------------
static void pwm_all_start(pwm_config_list_t *pwm_config_list);
static void pwm_all_set_duty_cycle_percent(pwm_config_list_t *pwm_config_list);
static void pwm_set_threshold(pwm_config_list_t *pwm_config_list);
static void pwm_all_init(pwm_config_list_t *pwm_config_list) ;
//--------------------------------------------function prototypes---------------------------------------------------


static void pwm_all_start(pwm_config_list_t *pwm_config_list) { //start all PWMs
	for(int i = 0; i < PWM_NUMBER_MAX; i++) pwm_start(&(pwm_config_list->pwm_cfg[i]), pwm_config_list->pwm_duty_cycle[i]); 
}

static void pwm_all_set_duty_cycle_percent(pwm_config_list_t *pwm_config_list){ //set all duty cycles for the pwm config structs
	for(int i = 0; i < PWM_NUMBER_MAX; i++) pwm_set_duty_cycle_percent(&(pwm_config_list->pwm_cfg[i]), pwm_config_list->pwm_duty_cycle[i]);
}


static void pwm_set_threshold(pwm_config_list_t *pwm_config_list) {
	fram_write_pwm(pwm_config_list); //write pwm duty cycle from ram to NVM
	pwm_all_set_duty_cycle_percent(pwm_config_list); //set pwm threshold
}


static void pwm_all_init(pwm_config_list_t *pwm_config_list) // initial all pwms
{
	fram_load_pwm(pwm_config_list);
	
	for (int i = 0; i < PWM_NUMBER_MAX; i++) { //write pwm frequency and default pwm duty cycle if input cycle is FF
		if(pwm_config_list->pwm_duty_cycle[i] == 0xFF) pwm_config_list->pwm_duty_cycle[i] = PWM_DUTY_DEFAULT;
		pwm_config_list->pwm_frequency_list[i] = PWM_FREQUENCY;
	}
	// initialize pwm
	pwm_init(&(pwm_config_list->pwm_cfg[0]), PWM_TCC0, PWM_CH_A, pwm_config_list->pwm_frequency_list[0]);
	pwm_init(&(pwm_config_list->pwm_cfg[1]), PWM_TCC0, PWM_CH_B, pwm_config_list->pwm_frequency_list[1]);
	pwm_init(&(pwm_config_list->pwm_cfg[2]), PWM_TCC0, PWM_CH_C, pwm_config_list->pwm_frequency_list[2]);
	pwm_init(&(pwm_config_list->pwm_cfg[3]), PWM_TCC0, PWM_CH_D, pwm_config_list->pwm_frequency_list[3]);
	pwm_init(&(pwm_config_list->pwm_cfg[4]), PWM_TCD0, PWM_CH_A, pwm_config_list->pwm_frequency_list[4]);
	pwm_init(&(pwm_config_list->pwm_cfg[5]), PWM_TCD0, PWM_CH_B, pwm_config_list->pwm_frequency_list[5]);
	pwm_init(&(pwm_config_list->pwm_cfg[6]), PWM_TCD0, PWM_CH_C, pwm_config_list->pwm_frequency_list[6]);
	pwm_init(&(pwm_config_list->pwm_cfg[7]), PWM_TCD0, PWM_CH_D, pwm_config_list->pwm_frequency_list[7]);
	pwm_init(&(pwm_config_list->pwm_cfg[8]), PWM_TCE0, PWM_CH_A, pwm_config_list->pwm_frequency_list[8]);
	pwm_init(&(pwm_config_list->pwm_cfg[9]), PWM_TCE0, PWM_CH_B, pwm_config_list->pwm_frequency_list[9]);
	pwm_init(&(pwm_config_list->pwm_cfg[10]), PWM_TCE0, PWM_CH_C, pwm_config_list->pwm_frequency_list[10]);
	pwm_init(&(pwm_config_list->pwm_cfg[11]), PWM_TCE0, PWM_CH_D, pwm_config_list->pwm_frequency_list[11]);
	pwm_init(&(pwm_config_list->pwm_cfg[12]), PWM_TCF0, PWM_CH_A, pwm_config_list->pwm_frequency_list[12]);
	pwm_init(&(pwm_config_list->pwm_cfg[13]), PWM_TCF0, PWM_CH_B, pwm_config_list->pwm_frequency_list[13]);
	pwm_init(&(pwm_config_list->pwm_cfg[14]), PWM_TCF0, PWM_CH_C, pwm_config_list->pwm_frequency_list[14]);
	pwm_init(&(pwm_config_list->pwm_cfg[15]), PWM_TCF0, PWM_CH_D, pwm_config_list->pwm_frequency_list[15]);

	pwm_all_start(pwm_config_list); //start pwm
}

#endif
//------------------------------------------------pwm application---------------------------------------------------

