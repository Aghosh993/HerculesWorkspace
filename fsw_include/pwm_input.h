#ifndef Quadcopter_PWM_Input_HAL_H_
#define Quadcopter_PWM_Input_HAL_H_		1

#include "pwm_input_hal.h"

typedef struct {
	float roll_joystick_val;
	float pitch_joystick_val;
	float yaw_joystick_val;
	float height_joystick_val;
} rc_joystick_data_struct;

#endif