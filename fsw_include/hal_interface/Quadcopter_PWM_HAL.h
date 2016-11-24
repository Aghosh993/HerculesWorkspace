/*
	File: Quadcopter_PWM_HAL.h

	(c) Abhimanyu Ghosh, 2016
 */

#ifndef QUADCOPTER_PWM_HAL_H_
#define QUADCOPTER_PWM_HAL_H_	1

#include <stdint.h>

#include <hal_common_includes.h>

#define MIN_DUTY 0.0f
#define MAX_DUTY 1.0f

// #define MOTOR1_HET_CHANNEL	hetRAM1, pwm0
// #define MOTOR2_HET_CHANNEL	hetRAM2, pwm2
// #define MOTOR3_HET_CHANNEL	hetRAM1, pwm2
// #define MOTOR4_HET_CHANNEL	hetRAM1, pwm1
#define MOTOR1_HET_CHANNEL	hetRAM1, pwm0
#define MOTOR2_HET_CHANNEL	hetRAM1, pwm1
#define MOTOR3_HET_CHANNEL	hetRAM1, pwm2
#define MOTOR4_HET_CHANNEL	hetRAM2, pwm0

// This configures all vehicle PWM channels:
void QuadRotor_PWM_init(void);

// Functions to initialize PWM channels corresponding to individual motor/ESC's
void QuadRotor_motor1_start(void);
void QuadRotor_motor2_start(void);
void QuadRotor_motor3_start(void);
void QuadRotor_motor4_start(void);

// Functions to stop PWM channels corresponding to individual motor/ESC's
void QuadRotor_motor1_stop(void);
void QuadRotor_motor2_stop(void);
void QuadRotor_motor3_stop(void);
void QuadRotor_motor4_stop(void);

// Functions to set PWM channels corresponding to individual motor/ESC's
void QuadRotor_motor1_setDuty(float duty);
void QuadRotor_motor2_setDuty(float duty);
void QuadRotor_motor3_setDuty(float duty);
void QuadRotor_motor4_setDuty(float duty);

#endif