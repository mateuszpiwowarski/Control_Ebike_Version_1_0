/*
 * page.h
 *
 *  Created on: Mar 11, 2024
 *      Author: mateu
 */

#ifndef INC_PAGE_H_
#define INC_PAGE_H_



typedef enum
{
    PAGE_ONE,
    PAGE_TWO,
    PAGE_THREE,
    PAGE_FOUR,
    NUM_PAGES
} PAGE;


extern PAGE current_page;


void load_page_one(float batteryVoltage, float current);
void load_page_two(float temperature, float speed);
void load_page_three(float totalDistance, float power);
void load_page_four(float temp);
void default_error_page(void);
void ButtonPressedCallback(PAGE *current_page);
#endif /* INC_PAGE_H_ */
