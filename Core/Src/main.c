/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "acs758.h"
#include "Battery.h"
#include "buttons.h"
#include "ds18B20.h"
#include "ssd1306_conf.h"
#include "ssd1306_tests.h"
#include "ssd1306_fonts.h"
#include "ssd1306.h"
#include "gps_neo6.h"
#include "hall.h"
#include "MCP4725.h"
#include "onewire.h"
#include "pid.h"
#include "ThrottleADC.h"
#include "page.h"
#include "lm35.h"
#include "ntc10k.h"
#include "SwitchSpeed.h"

#include "stdint.h"
#include "stdio.h"
#include <math.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

///// NTC10K ////
uint16_t ADC_Raw;
float Ntc_Tmp;

uint8_t Sch_100ms = 255;

///// LM35 /////
float temp;
uint32_t adcval;

//// POWER  /////
float power;

/// ACS758 /////
double current;
float voltage;
uint32_t value;

//// Throttle /////
MCP4725 myDAC;
float averageVoltage;
uint32_t adc_val;

//// V_BAT /////
uint32_t bat_adc_val;
float batteryVoltage;

/// NEO6GPS  /////
NEO6_State GpsState;

/// BUTTON ////
BUTTON_T BUTTON1_Pin;
BUTTON_T BUTTON2_Pin;
BUTTON_T BUTTON3_Pin;

/// DS18B20/////
float temperature;

////HALL_SPEED /////
extern volatile float speed;
extern volatile float rotational_speed;
extern volatile uint32_t pulseCounter;
extern volatile uint32_t lastImpulseTime;
extern volatile uint32_t lastPulseCheckTime;
extern volatile float totalDistance;

//// SSD1306 PAGE ////
uint8_t pageIndex = 0; // This variable will keep track of which page should be displayed.
uint8_t supportLevel = 1; // Start at level 1

//// NEO6GPS /////
NEO6_State GpsState;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void renderPage1(char* buffer);
void renderPage2(char* buffer);
void renderPage3(char* buffer);
void renderPage4(char* buffer);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

////////////////////////////// DS18B20 //////
  uint8_t sensor_count = 0;  // Variable_number of sensor
  uint8_t ROM_tmp[2];		//  Array_sensor address

  DS18B20_Init(DS18B20_Resolution_12bits);  // Initialization
  sensor_count = DS18B20_Quantity();

///////////////////////////////// NTC10K
  HAL_TIM_Base_Start_IT(&htim4);


///////////////////////////////NEO6GPS
  NEO6_Init(&GpsState, &huart1);

////////////////////////////// ACS758 /////
  ACS7XX Serch;
  ACS7XX_ResetCounters(&Serch);
  ACS7XX_Calibrate(&Serch);
  ACS7XX_Init_Default(&Serch);

////////////////////////////// Throttle  //////
	/* Initialize DAC */
  myDAC = MCP4725_init(&hi2c2, MCP4725A0_ADDR_A00, MCP4725_REFERENCE_VOLTAGE);

	/* Check connection with DAC */
  if (!MCP4725_isConnected(&myDAC))
  {
	  HAL_GPIO_WritePin(LED_Pin_GPIO_Port, LED_Pin_Pin, GPIO_PIN_SET);
  }
  else
  {
	  HAL_GPIO_WritePin(LED_Pin_GPIO_Port, LED_Pin_Pin, GPIO_PIN_RESET);
  }

///////////////////////////// HALLSPEED /////
  HAL_TIM_IC_START();
//  IC_Capture_Callback(&htim2);

///////////////////////////// SSD1306 /////
  ssd1306_Init();

//////////////////////////// NEO6GPS  //////
  NEO6_Init(&GpsState, &huart1);

/////////////////////////////// BUTTON ////
  Button_Init(&BUTTON1_Pin, BUTTON1_Pin_GPIO_Port, BUTTON1_Pin_Pin, 20, 2000, 500);
  Button_Init(&BUTTON2_Pin, BUTTON2_Pin_GPIO_Port, BUTTON2_Pin_Pin, 30, 2000, 500);
  Button_Init(&BUTTON3_Pin, BUTTON3_Pin_GPIO_Port, BUTTON3_Pin_Pin, 40, 2000, 500);

///////////////////////////////// PWM /////

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

/////////////////////////////// DELAY ////
  uint32_t DS18B20_delay = 0;
  uint32_t POWER_delay = 0;
  uint32_t ACS758_delay = 0;
  uint32_t V_BAT_delay = 0;
  uint32_t Throttle_delay = 0;
  uint32_t SSD1306_delay = 0;
  uint32_t NEO6GPS_delay = 0;
  uint32_t BUTTON_delay = 0;
  uint32_t LM35_delay = 0;
  uint32_t NEO6_delay = 0;
  uint32_t NTC10K_delay = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

//////////////////////////////////////  DS18B20 //////
	if((HAL_GetTick() - DS18B20_delay) > 100)
	{
	  if(sensor_count > 0)
	  {
	  	DS18B20_GetROM(0, ROM_tmp);
	  	DS18B20_AllDone();
	  	DS18B20_ReadAll();
	  	DS18B20_StartAll();
	  	DS18B20_GetTemperature(0, &temperature);

	  }
	 DS18B20_delay = HAL_GetTick();
	}


////////////////////////////////// PWM THROTTLE////////////
	Throttle_Control_PWM( &hadc1, &htim3, TIM_CHANNEL_2);


//////////////////////////////////////NTC10K /////////
	if((HAL_GetTick() - NTC10K_delay) > 150)
	{
	  if(Sch_100ms)
	    {
		  NTC10K_ADC_Read(&ADC_Raw);
		  NTC10K_Get_Temp(&Ntc_Tmp);

	    }
	  NTC10K_delay = HAL_GetTick();
	}

//////////////////////////////////////HALLSPEED /////
// Check if TIMEOUT has passed since last pulse check
	if (HAL_GetTick() - lastPulseCheckTime >= TIMEOUT)
	{
		// If the last pulse was more than TIMEOUT milliseconds ago
		if (HAL_GetTick() - lastImpulseTime >= TIMEOUT)
		{
		  // reset speed, rotational_speed and pulseCounter
			pulseCounter = 0;
			speed = 0;
			rotational_speed = 0;
		}

		// Update the last pulse check time
		lastPulseCheckTime = HAL_GetTick();
	}
////////////////////////////////////////// LM35 ///////
	if((HAL_GetTick() - LM35_delay) > 200)
	{
		LM35_Get_Value(&hadc1, &adcval);
		LM35_Get_Temperature(&hadc1, &temp);

		LM35_delay = HAL_GetTick();
	}

//////////////////////////////////////////// POWER //////
	if((HAL_GetTick() - POWER_delay) > 100)
	{
		Power_Calculate(&Serch, &hadc1, &power);

		POWER_delay = HAL_GetTick();
	}

//////////////////////////////////////////// ACS758 /////
	if((HAL_GetTick() - ACS758_delay) > 100)
	{
	   ACS7XX_InstantCurrent(&Serch, &current, &voltage, &value);

	   ACS758_delay = HAL_GetTick();
	}

///////////////////////////////////////////// V_BAT ///////
	if((HAL_GetTick() - V_BAT_delay) > 100)
	{
	   HAL_ADC_Start(&hadc1);
	   HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	   Battery_Get_ADC_Value(&hadc1, &bat_adc_val);
	   Battery_Get_Voltage(&hadc1 , &batteryVoltage);
	   HAL_ADC_Stop(&hadc1);

	   V_BAT_delay = HAL_GetTick();
	}

//////////////////////////////////////////// Throttle /////
	if((HAL_GetTick() - Throttle_delay) > 100)
	{
	   Throttle_Get_Average_Voltage(&hadc1, &averageVoltage);
	   Throttle_Get_ADC_Value(&hadc1, &adc_val);
	   if(!MCP4725_setVoltage(&myDAC, averageVoltage , MCP4725_FAST_MODE, MCP4725_POWER_DOWN_OFF))
	   {
	      /* Handle error */
	   }
	 Throttle_delay = HAL_GetTick();
	}

//////////////////////////////////////////NEO6GPS
	if((HAL_GetTick() - NEO6_delay) > 100)
	{
	  NEO6_Task(&GpsState);

  	  NEO6_delay = HAL_GetTick();
	}
////////////////////////////////////////////// SSD1306  ////

	if((HAL_GetTick() - SSD1306_delay) > 100)
	{
		char displayBuffer[50];
		ssd1306_Fill(Black);

		// Depending on the page to be displayed we render different information.
		switch (pageIndex)
		{
			case 0: // Page 1
		      renderPage1(displayBuffer);
		      break;

			case 1: // Page 2
		      renderPage2(displayBuffer);
		      break;

			case 2: // Page 3
		      renderPage3(displayBuffer);
		      break;

			case 3: // Page 4
		      renderPage4(displayBuffer);
		      break;

			default:
				// If an unrecognized page index is detected, reset to page 1.
				pageIndex = 0;
			}
		ssd1306_UpdateScreen();

		SSD1306_delay = HAL_GetTick();
	}

///////////////////////////////////////////////NEO6GPS ////
	if((HAL_GetTick() - NEO6GPS_delay) > 1000)
	{
		NEO6_Task(&GpsState);
		NEO6_IsFix(&GpsState);

		NEO6GPS_delay = HAL_GetTick();
	}


////////////////////////////////////////////// BUTTON ////
	if((HAL_GetTick() - BUTTON_delay) > 100)
	{

		/////// If button 3 is pressed, we move to the next page.
		if (Button_IsPressed(&BUTTON1_Pin))
		{
			// If we were in the last page, reset to page 1.
		    if (pageIndex >= 3)
		    {
		    	pageIndex = 0;
		    }
		    // Otherwise, simply go to the next page.
		    else
		    {
		    	pageIndex++;
		    }
		}

		/////// If button 2 is pressed, increment the support level.
		if (Button_IsPressed(&BUTTON2_Pin))
		{
			if (supportLevel <= 4)
		    {
		        supportLevel++;
		    }
		}

		/////// If button 3 is pressed, decrement the support level.
		if (Button_IsPressed(&BUTTON3_Pin))
		{
			if (supportLevel >= 1)
		    {
				supportLevel--;
		    }
		}
		   BUTTON_delay = HAL_GetTick();
	}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/////////////////////////////////////// SSD1306PAGE ///////
void renderPage1(char* buffer)
{
	char displayBuffer[50];

	// Display Speed
	ssd1306_SetCursor(45, 30);
	sprintf(displayBuffer, "%.2f km/h", speed);
	ssd1306_WriteString(displayBuffer, Font_7x10, White);
    // Display battery voltage
    ssd1306_SetCursor(10, 2);
    sprintf(displayBuffer, ": %.1f V", batteryVoltage);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display POWER
    ssd1306_SetCursor(84, 2);
    sprintf(displayBuffer, "%.1f W", power);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display temperature DS18B20
    ssd1306_SetCursor(80, 55);
    sprintf(displayBuffer, "%.1f C", temperature);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display distance
    ssd1306_SetCursor(2, 55);
    sprintf(displayBuffer, "%.3f km", totalDistance);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    //Display support level
    ssd1306_SetCursor(5, 25);
    snprintf(displayBuffer, sizeof(displayBuffer), "%u", supportLevel);
    ssd1306_WriteString(displayBuffer, Font_11x18, White);

    // Calculate the height of the temperature bar
    int tempPercent = temperature; // This assumes temperature is between 0-100. If not, make sure to scale it.
     // Check if temperature is in range.
     if(tempPercent < 0)
     {
       tempPercent = 0;
     }
     else if(tempPercent > 100)
     {
       tempPercent = 100;
     }
     // Calculate height for temperature.
     int tempHeight = (int) (52*tempPercent/100); // Assuming the height of OLED is 64 pixels. Adjust this value according to your OLED resolution.
     // Draw temperature bar.
     for(int y=63; y>=0; y--)
     {
       for(int x=124; x<128; x++)
       { // Assuming the width of OLED is 128 pixels.
         if(y >= (64-tempHeight))
         {
           ssd1306_DrawPixel(x, y, White); // Draw the bar as white.
         }
         else
         {
           ssd1306_DrawPixel(x, y, Black); // Other area remains black.
         }
       }
     }

 	// Calculate battery level percentage.
 	float batPercent = (((float)batteryVoltage - 20) / 10) * 100 ;    // Assuming batteryVoltage ranges from 10V to 30V.
 	// Check if battery level is in range.
 	if(batPercent < 0)
 	{
 		batPercent = 0;
 	}
 	else if(batPercent > 100)
 	{
 	    batPercent = 100;
 	}

     uint8_t batteryWidth = 15;
     uint8_t batteryHeight = 7;
     uint8_t topLeftX = 2; // top right corner (leave 2 pixel space from the edge)
     uint8_t topLeftY = 2; // top right corner (leave 2 pixel space from the edge)
     // draw battery body
     ssd1306_DrawRectangle(topLeftX, topLeftY, topLeftX + batteryWidth, topLeftY + batteryHeight, White);
     // draw battery terminal
     ssd1306_Line(topLeftX + batteryWidth + 1, topLeftY + 1, topLeftX + batteryWidth + 1, topLeftY + batteryHeight - 1, White);
     // you can adjust the width of the filled rectangle reflects battery level
     // pseudocode: int batteryLevelWidth = batteryWidth * (currentBatteryLevel / maxBatteryLevel);
     int batteryLevelWidth =  (batPercent * batteryWidth) / 100; // let's say it's 70% now
     ssd1306_FillRectangle(topLeftX + 1, topLeftY + 1, topLeftX + 1 + batteryLevelWidth, topLeftY + batteryHeight - 1, White);
}

void renderPage2(char* buffer)
{
	char displayBuffer[50];
  // Display battery voltage
	ssd1306_SetCursor(2, 0);
	sprintf(displayBuffer, "V_Battery: %.2f V", batteryVoltage);
	ssd1306_WriteString(displayBuffer, Font_6x8, White);

  // Display current
	ssd1306_SetCursor(2, 8);
	sprintf(displayBuffer, "Current: %.2f A", current);
	ssd1306_WriteString(displayBuffer, Font_6x8, White);

  // Display Speed
	ssd1306_SetCursor(2, 16);
	sprintf(displayBuffer, "Speed: %.2f km/h", speed);
	ssd1306_WriteString(displayBuffer, Font_6x8, White);

  // Display distance
	ssd1306_SetCursor(2, 24);
	sprintf(displayBuffer, "Distance: %.2f km", totalDistance);
	ssd1306_WriteString(displayBuffer, Font_6x8, White);

  // Display POWER
	ssd1306_SetCursor(2, 32);
	sprintf(displayBuffer, "Power: %.2f W", power);
	ssd1306_WriteString(displayBuffer, Font_6x8, White);

  // Display LM35
	ssd1306_SetCursor(2, 40);
	sprintf(displayBuffer, "TempLM35: %.2f *C", temp);
	ssd1306_WriteString(displayBuffer, Font_6x8, White);

  // Display NTC10K
	ssd1306_SetCursor(2, 48);
	sprintf(displayBuffer, "TempNTC10K: %.2f *C", Ntc_Tmp);
	ssd1306_WriteString(displayBuffer, Font_6x8, White);

  // Display temperature
	ssd1306_SetCursor(2, 56);
	sprintf(displayBuffer, "TempDS18B20: %.2f *C", temperature);
	ssd1306_WriteString(displayBuffer, Font_6x8, White);
}

void renderPage3(char* buffer){
	char displayBuffer[50];

    ssd1306_SetCursor(10, 0);
    sprintf(displayBuffer, "GPS INFORMATION");
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display GPS satelites number
    ssd1306_SetCursor(2, 9);
    sprintf(displayBuffer, "SatelitesNumber: %d", GpsState.SatelitesNumber);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display GPS Date
    ssd1306_SetCursor(2, 18);
    sprintf(displayBuffer, "Date: %d.%d.%d", GpsState.Day, GpsState.Month, GpsState.Year);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display GPS Time
    ssd1306_SetCursor(2, 27);
    sprintf(displayBuffer, "Time: %d:%d:%d", GpsState.Hour, GpsState.Minute, GpsState.Second);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display GPS Speed
    ssd1306_SetCursor(2, 36);
    sprintf(displayBuffer, "Speed: %.2f km/h", GpsState.SpeedKilometers);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display GPS Latitude and Longitude
    ssd1306_SetCursor(2, 45);
    sprintf(displayBuffer, "Lat: %f", GpsState.Latitude);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    ssd1306_SetCursor(2, 54);
    sprintf(displayBuffer, "Long: %f",GpsState.Longitude);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);
//
//    // Display GPS Latitude and Longitude
//    ssd1306_SetCursor(2, 45);
//    sprintf(displayBuffer, "Lat: **.******");
//    ssd1306_WriteString(displayBuffer, Font_6x8, White);
//
//    ssd1306_SetCursor(2, 54);
//    sprintf(displayBuffer, "Long: **.****** ");
//    ssd1306_WriteString(displayBuffer, Font_6x8, White);
}

void renderPage4(char* buffer){
	char displayBuffer[50];

    ssd1306_SetCursor(35, 2);
    sprintf(displayBuffer, "SETTINGS");
    ssd1306_WriteString(displayBuffer, Font_7x10, White);

    ssd1306_SetCursor(0, 15);
    sprintf(displayBuffer, "-Temperature_SENS ");
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    ssd1306_SetCursor(0, 30);
    sprintf(displayBuffer, "-Current_SENS_CALIB");
    ssd1306_WriteString(displayBuffer, Font_6x8, White);
}

/////////////////////////////////////////////// NEO6GPS  //////
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == GpsState.neo6_huart)
	{
		NEO6_ReceiveUartChar(&GpsState);
	}
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
