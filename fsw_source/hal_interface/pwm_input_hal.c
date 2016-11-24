#include "pwm_input_hal.h"

void pwmGetSignalHigherPrecision(hetRAMBASE_t * hetRAM, uint32 cap, pwm_info_t *pwm)
{
    uint32    pwmDuty   = (hetRAM->Instruction[(cap << 1U) + 25U].Data) >> 7U;
    uint32    pwmPeriod = (hetRAM->Instruction[(cap << 1U) + 26U].Data) >> 7U;

    pwm->duty_us   = pwmDuty;

    if( hetRAM == hetRAM1)
    {
        pwm->period_us = (uint32_t)(((float64)pwmPeriod * 1292.799F) / 1000.0F);
    }
    else
    {
        pwm->period_us = (uint32_t)(((float64)pwmPeriod * 1292.799F) / 1000.0F);
    }
}