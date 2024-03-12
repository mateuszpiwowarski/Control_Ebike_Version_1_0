/*
 * ThrottleADC.c
 *
 *  Created on: Feb 13, 2024
 *      Author: mateu
 */

#include "ThrottleADC.h"
#include "adc.h"


void Throttle_Get_ADC_Value(ADC_HandleTypeDef* hadc, uint32_t* adc_val)
{
	ADC_Select_CH9();
	HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
    *adc_val = HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop(hadc);
}

void Throttle_Get_Voltage(ADC_HandleTypeDef* hadc, float* outputVoltage)
{
	uint32_t adc_val;
	Throttle_Get_ADC_Value(hadc, &adc_val);
    *outputVoltage = (((float)adc_val - 1000) / ADC_RESOLUTION) * (VMAX - VMIN) + VMIN;
}

void Throttle_Get_Average_Voltage(ADC_HandleTypeDef* hadc, float* averageVoltage)
{
    float sum_voltage = 0;
    float single_voltage;
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        Throttle_Get_Voltage(hadc, &single_voltage);
        sum_voltage += single_voltage;
    }
    *averageVoltage = sum_voltage / NUM_SAMPLES;
}
