
#include "hall.h"
#include "stdint.h"
#include "stdio.h"
#include "tim.h"
#include "math.h"
#include "stm32f4xx_hal_tim.h"


void HAL_TIM_IC_START(void)
{
	  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
}


volatile uint32_t lastImpulseTime = 0;
volatile uint32_t lastPulseCheckTime = 0;
volatile uint32_t pulseCounter = 0;
volatile uint32_t startTime = 0;
volatile float elapsedTime = 0;
volatile float speed;
volatile float rotational_speed;
volatile float totalDistance;
float elapsedTimeHours;



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
/* Instantiate for TIM3 */
	if(htim->Instance == TIM2)
	{
	// For both rising and falling edges
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1 || htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			if (HAL_GetTick() - lastImpulseTime >= TIMEOUT)
		    {
		    	pulseCounter = 0;
		    	speed = 0;
		    	rotational_speed = 0;
		    }

			uint32_t now = HAL_GetTick();
			if(now - lastImpulseTime > DEBOUNCE_DELAY) // If enough time has passed since last impulse
				{
					if(pulseCounter == 0)
					{
						startTime = now;
					}
					pulseCounter++;
					lastImpulseTime = now; // Update the time of the last registered impulse
					// Check if we have 30 pulses
			if (pulseCounter == 30)
			{
	        // Compute elapsed time
	        elapsedTime = HAL_GetTick() - startTime;

	        // Compute rotational speed (RPS)
	        float elapsedTimeSec = elapsedTime / 500.0;
	        rotational_speed = ((60 / elapsedTimeSec) / 30);
	        rotational_speed = rotational_speed * 60; // Convert to RPM (rotations per minute)

	        // Compute linear speed
	        float diameter_in_meters = 8 * 0.0254; // Diameter in meters (8 inches -> ~0.2032m)
	        float circumference = diameter_in_meters * M_PI;
	        speed = circumference * (rotational_speed / 60); // Convert RPM to RPS and calculate speed in m/s
	        speed = speed * 3.6; // Convert speed to km/h

	        // Compute distance for this set of 30 pulses
	        elapsedTimeHours = elapsedTime / 3600000.0; // Convert elapsed time to hours
	        float pulseDistance = speed * elapsedTimeHours; // distance for this set of 30 pulses in km

	    	// Add the pulse distance to the total distance
	    	totalDistance += pulseDistance;

	        // Reset pulse counter
	        pulseCounter = 0;

	        // Start new time measurement
	        startTime = 0;

	      }
	      lastImpulseTime = HAL_GetTick();
	      // Restart measure for both channels
	      __HAL_TIM_SetCounter(htim, 0);

	      if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	      {
	        HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_1);
	      }
	      else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	      {
	        HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_2);
	      }
	    }
	  }
	}
}
