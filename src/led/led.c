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

#define RED_LED_PIN 6
#define YELLOW_LED_PIN 0
#define GREEN_LED_PIN 1
#define STATUS_LED_PIN 0

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void led_all_toggle()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	led_red_toggle();
	led_yellow_toggle();
	led_green_toggle();
}

void led_red_toggle()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	TOGGLE_BIT(GPIO_PORTD_DATA_R, RED_LED_PIN);
}

void led_yellow_toggle()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	TOGGLE_BIT(GPIO_PORTG_DATA_R, YELLOW_LED_PIN);
}

void led_green_toggle()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	TOGGLE_BIT(GPIO_PORTG_DATA_R, GREEN_LED_PIN);
}

void led_status_toggle()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	TOGGLE_BIT(GPIO_PORTF_DATA_R, STATUS_LED_PIN);
}

void led_status_off()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	CLEAR_BIT(GPIO_PORTF_DATA_R, STATUS_LED_PIN);
}

void led_status_on()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	SET_BIT(GPIO_PORTF_DATA_R, STATUS_LED_PIN);
}

void led_yellow_on()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	CLEAR_BIT(GPIO_PORTG_DATA_R, YELLOW_LED_PIN);
}

void led_yellow_off()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	SET_BIT(GPIO_PORTG_DATA_R, YELLOW_LED_PIN);
}

void led_green_on()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	CLEAR_BIT(GPIO_PORTG_DATA_R, GREEN_LED_PIN);
}

void led_green_off()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	SET_BIT(GPIO_PORTG_DATA_R, GREEN_LED_PIN);
}

void led_red_on()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	CLEAR_BIT(GPIO_PORTD_DATA_R, RED_LED_PIN);
}

void led_red_off()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	SET_BIT(GPIO_PORTD_DATA_R, RED_LED_PIN);
}

void init_leds()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U dummy;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOG | SYSCTL_RCGC2_GPIOF; // Enable the port for the red LED
	dummy = SYSCTL_RCGC2_R;

	SET_BIT(GPIO_PORTD_DIR_R, RED_LED_PIN);
	SET_BIT(GPIO_PORTD_DEN_R, RED_LED_PIN);
	SET_BIT(GPIO_PORTG_DIR_R, GREEN_LED_PIN);
	SET_BIT(GPIO_PORTG_DEN_R, GREEN_LED_PIN);
	SET_BIT(GPIO_PORTG_DIR_R, YELLOW_LED_PIN);
	SET_BIT(GPIO_PORTG_DEN_R, YELLOW_LED_PIN);
	SET_BIT(GPIO_PORTF_DIR_R, STATUS_LED_PIN);
	SET_BIT(GPIO_PORTF_DEN_R, STATUS_LED_PIN);

	led_red_off();
	led_yellow_off();
	led_green_off();
	led_status_off();
}

/****************************** End Of Module *******************************/
