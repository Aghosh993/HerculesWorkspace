#ifndef PWM_INPUT_HAL_H_
#define PWM_INPUT_HAL_H_	1

#include "hal_common_includes.h"

typedef struct {
	uint32_t duty_us;
	uint32_t period_us;
} pwm_info_t;

void pwmGetSignalHigherPrecision(hetRAMBASE_t * hetRAM, uint32_t cap, pwm_info_t *pwm);

#endif