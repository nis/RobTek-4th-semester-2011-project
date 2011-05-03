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
//#include "../inc/glob_def.h"
#include "../inc/binary.h"

/*****************************    Defines    *******************************/

#define UP_PIN 0
#define DOWN_PIN 1

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT8U up_clicks 	= 0;
INT8U down_clicks 	= 0;

/*****************************   Functions   *******************************/

INT8U button_up_pushed()
{
  return( TEST_BIT_LOW(GPIO_PORTE_DATA_R, UP_PIN) );
}

INT8U button_down_pushed()
{
  return( TEST_BIT_LOW(GPIO_PORTE_DATA_R, DOWN_PIN) );
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
	SET_BIT(GPIO_PORTE_DEN_R, UP_PIN); // Up
	SET_BIT(GPIO_PORTE_DEN_R, DOWN_PIN); // Down
	
	// Enable pull-up
	SET_BIT(GPIO_PORTE_PUR_R, UP_PIN); // Up
	SET_BIT(GPIO_PORTE_PUR_R, DOWN_PIN); // Down
}

void button_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	static INT8U up_state 	= 0; // 0 = not pushed, 1 = button pressed down
	static INT8U down_state = 0; // Same
	
	// Up button
	if (up_state == 0) {
		if(button_up_pushed())
		{
			up_state = 1;
			up_clicks++;
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
			down_clicks++;
		}
	} else {
		if(!button_down_pushed())
		{
			down_state = 0;
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



/****************************** End Of Module *******************************/
