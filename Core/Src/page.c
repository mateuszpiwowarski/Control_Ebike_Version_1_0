/*
 * page.c
 *
 *  Created on: Mar 11, 2024
 *      Author: mateu
 */
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

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

#include "stdint.h"
#include "stdio.h"
#include <math.h>
#include <string.h>

extern PAGE current_page;


void load_page_one(float batteryVoltage, float current)
{
    char displayBuffer[50];

    ssd1306_Fill(Black);

    // Display battery voltage
    ssd1306_SetCursor(2, 0);
    sprintf(displayBuffer, "V_Battery: %.2f V", batteryVoltage);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display current
    ssd1306_SetCursor(2, 8);
    sprintf(displayBuffer, "Current: %.2f A", current);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);
}

void load_page_two(float temperature, float speed)
{
    char displayBuffer[50];

    ssd1306_Fill(Black);

    // Display temperature
    ssd1306_SetCursor(2, 0);
    sprintf(displayBuffer, "TempDS18B20: %.2f *C", temperature);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display speed
    ssd1306_SetCursor(2, 8);
    sprintf(displayBuffer, "Speed: %.2f km/h", speed);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);
}

void load_page_three(float totalDistance, float power)
{
    char displayBuffer[50];

    ssd1306_Fill(Black);

    // Display distance
    ssd1306_SetCursor(2, 0);
    sprintf(displayBuffer, "Distance: %.2f km", totalDistance);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Display power
    ssd1306_SetCursor(2, 8);
    sprintf(displayBuffer, "Power: %.2f W", power);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);
}

void load_page_four(float temp)
{
    char displayBuffer[50];

    ssd1306_Fill(Black);

    // Display LM35
    ssd1306_SetCursor(2, 0);
    sprintf(displayBuffer, "TempLM35: %.2f *C", temp);
    ssd1306_WriteString(displayBuffer, Font_6x8, White);

    // Cześć kodu odpowiedzialna za wyświetlenie wskaźnika stanu baterii może zostać tutaj
}


void ButtonPressedCallback(PAGE *current_page)
{
    // gdy przycisk jest naciśnięty, zmieniamy stronę
    *current_page = (*current_page + 1) % NUM_PAGES;

    // teraz możemy zaktualizować ekran OLED
    switch (*current_page)
    {
        case PAGE_ONE:
            // załaduj funkcję odpowiedzialną za stronę 1
        	void load_page_one(float batteryVoltage, float current);
            break;

        case PAGE_TWO:
            // załaduj funkcję odpowiedzialną za stronę 2
        	void load_page_two(float temperature, float speed);
            break;

        case PAGE_THREE:
            // załaduj funkcję odpowiedzialną za stronę 3
        	void load_page_three(float totalDistance, float power);
            break;

        case PAGE_FOUR:
            // załaduj funkcję odpowiedzialną za stronę 4
        	void load_page_four(float temp);
            break;

        default:
            // w przypadku błędu
            default_error_page();
            break;
    }
    ssd1306_UpdateScreen();
}

void default_error_page() {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString("Error: Undefined page!", Font_6x8, White);
    ssd1306_UpdateScreen();
}
