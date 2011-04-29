/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: cpu.c
*
* PROJECT....: ECP
*
* DESCRIPTION:
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "inc/lm3s6965.h"
#include "../inc/emp_type.h"
#include "../inc/binary.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "../defines.h"

/*****************************    Defines    *******************************/

#define UP_PIN 		0
#define DOWN_PIN 	1
#define LEFT_PIN 	2
#define RIGHT_PIN 	3
#define SELECT_PIN	1

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT8U up_clicks 	= 0;
INT8U down_clicks 	= 0;
INT8U left_clicks 	= 0;
INT8U right_clicks 	= 0;
INT8U select_clicks = 0;

/*****************************   Functions   *******************************/

INT8U button_up_pushed()
{
  return( TEST_BIT_LOW(GPIO_PORTE_DATA_R, UP_PIN) );
}

INT8U button_down_pushed()
{
  return( TEST_BIT_LOW(GPIO_PORTE_DATA_R, DOWN_PIN) );
}

INT8U button_left_pushed()
{
  return( TEST_BIT_LOW(GPIO_PORTE_DATA_R, LEFT_PIN) );
}

INT8U button_right_pushed()
{
  return( TEST_BIT_LOW(GPIO_PORTE_DATA_R, RIGHT_PIN) );
}

INT8U button_select_pushed()
{
  return( TEST_BIT_LOW(GPIO_PORTF_DATA_R, SELECT_PIN) );
}

void init_buttons()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8S dummy;
	
	// Enable the GPIO port
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
	
	dummy = SYSCTL_RCGC2_R;
	
	// Enable digital function
	SET_BIT(GPIO_PORTE_DEN_R, UP_PIN);
	SET_BIT(GPIO_PORTE_DEN_R, DOWN_PIN);
	SET_BIT(GPIO_PORTE_DEN_R, LEFT_PIN);
	SET_BIT(GPIO_PORTE_DEN_R, RIGHT_PIN);
	SET_BIT(GPIO_PORTF_DEN_R, SELECT_PIN);
	
	// Enable pull-up
	SET_BIT(GPIO_PORTE_PUR_R, UP_PIN);
	SET_BIT(GPIO_PORTE_PUR_R, DOWN_PIN);
	SET_BIT(GPIO_PORTE_PUR_R, LEFT_PIN);
	SET_BIT(GPIO_PORTE_PUR_R, RIGHT_PIN);
	SET_BIT(GPIO_PORTF_PUR_R, SELECT_PIN);
}

void button_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	gasstation_event event;
	
	// 0 = not pushed, 1 = button pressed down
	static INT8U up_state 		= 0;
	static INT8U down_state 	= 0;
	static INT8U left_state 	= 0;
	static INT8U right_state 	= 0;
	static INT8U select_state 	= 0;
	
	// Statemachines for each button.
	
	// Up button
	if (up_state == 0) {
		if(button_up_pushed())
		{
			up_state = 1;
			//up_clicks++;
			event.event = EVENT_PRODUCT_CLICK;
			xQueueSend(event_queue, &event, portMAX_DELAY);
		}
	} else {
		if(!button_up_pushed())
		{
			up_state = 0;
		}
	}
	
	// Down button
	if (down_state == 0) {
		if(button_down_pushed())
		{
			down_state = 1;
			//down_clicks++;
			event.event = EVENT_ACCOUNT_CLICK;
			xQueueSend(event_queue, &event, portMAX_DELAY);
		}
	} else {
		if(!button_down_pushed())
		{
			down_state = 0;
		}
	}
	
	// left button
	if (left_state == 0) {
		if(button_left_pushed())
		{
			left_state = 1;
			//left_clicks++;
			event.event = EVENT_ADD_50_KR;
			xQueueSend(event_queue, &event, portMAX_DELAY);
		}
	} else {
		if(!button_left_pushed())
		{
			left_state = 0;
		}
	}
	
	// right button
	if (right_state == 0) {
		if(button_right_pushed())
		{
			right_state = 1;
			//right_clicks++;
			event.event = EVENT_ADD_100_KR;
			xQueueSend(event_queue, &event, portMAX_DELAY);
		}
	} else {
		if(!button_right_pushed())
		{
			right_state = 0;
		}
	}
	
	// select button
	if (select_state == 0) {
		if(button_select_pushed())
		{
			select_state = 1;
			//select_clicks++;
			event.event = EVENT_HANDLE_LIFTET;
			xQueueSend(event_queue, &event, portMAX_DELAY);
		}
	} else {
		if(!button_select_pushed())
		{
			select_state = 0;
			event.event = EVENT_HANDLE_REPLACED;
			xQueueSend(event_queue, &event, portMAX_DELAY);
		}
	}
}

INT8U get_down_clicks()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U return_value = down_clicks;
	down_clicks = 0;
	return return_value;
}

INT8U get_up_clicks()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U return_value = up_clicks;
	up_clicks = 0;
	return return_value;
}

INT8U get_left_clicks()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U return_value = left_clicks;
	left_clicks = 0;
	return return_value;
}

INT8U get_right_clicks()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U return_value = right_clicks;
	right_clicks = 0;
	return return_value;
}

INT8U get_select_clicks()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U return_value = select_clicks;
	select_clicks = 0;
	return return_value;
}

/****************************** End Of Module *******************************/
