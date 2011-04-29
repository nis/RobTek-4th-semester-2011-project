/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* PROJECT: 		
* AUTHOR: 		NS
* DESCRIPTION: 	See module specification file (.h-file).
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "../inc/lm3s6965.h"
#include "../inc/emp_type.h"
#include "../cpu/cpu.h"
#include "../inc/binary.h"
#include "keyboard.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "../defines.h"
#include "../lcd/lcd.h"
#include "../led/led.h"
/*****************************    Defines    *******************************/

// States
#define PRESSED_NONE 0
#define PRESSED_1 1
#define PRESSED_2 2
#define PRESSED_3 3
#define PRESSED_4 4
#define PRESSED_5 5
#define PRESSED_6 6
#define PRESSED_7 7
#define PRESSED_8 8
#define PRESSED_9 9
#define PRESSED_0 10
#define PRESSED_STAR 11
#define PRESSED_HASH 12

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT8U keyboard_state = PRESSED_NONE;

/*****************************   Functions   *******************************/


void init_keyboard(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
	INT8U dummy;
	
	//enables clock for portB
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
	dummy = SYSCTL_RCGC2_R;
	
	//sets pin 2-4 as output on PORTB
	GPIO_PORTB_DIR_R |= (1 << 4);
	GPIO_PORTB_DIR_R |= (1 << 3);
	GPIO_PORTB_DIR_R |= (1 << 2);
	//makes pin 2-4 digtal  on PORTB
	GPIO_PORTB_DEN_R |= (1 << 4);
	GPIO_PORTB_DEN_R |= (1 << 3);
	GPIO_PORTB_DEN_R |= (1 << 2);

	//enables clock for portD
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD;
	dummy = SYSCTL_RCGC2_R;
	
	//sets pin 2-5 as input on PORTD
	GPIO_PORTD_DIR_R &= ~(1 << 5);
	GPIO_PORTD_DIR_R &= ~(1 << 4);
	GPIO_PORTD_DIR_R &= ~(1 << 3);
	GPIO_PORTD_DIR_R &= ~(1 << 2);
	//makes pin 2-5 digtal on PORTD
	GPIO_PORTD_DEN_R |= (1 << 5);
	GPIO_PORTD_DEN_R |= (1 << 4);
	GPIO_PORTD_DEN_R |= (1 << 3);
	GPIO_PORTD_DEN_R |= (1 << 2);
}

INT8U read_keyboard( void ) 
/*****************************************************************************
*   Input    : 	-
*   Output   : 	ASCI code for sign pushed on keyboard
*   Function : 	read of multiplexed keyboard
******************************************************************************/
{
	INT8U dummy;
	
	SET_BIT(GPIO_PORTB_DATA_R, 2);
	CLEAR_BIT(GPIO_PORTB_DATA_R, 3);
	CLEAR_BIT(GPIO_PORTB_DATA_R, 4);
	dummy = GPIO_PORTB_DATA_R;
	
	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,5))
	{
		if (keyboard_state != PRESSED_1)
		{
			keyboard_state = PRESSED_1;
			return '1';
		}	 else {
				return 0;
			}
	}

	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,4))
	{
		if (keyboard_state != PRESSED_4)
		{
			keyboard_state = PRESSED_4;
			return '4';
		}	 else {
				return 0;
			}
	}
	
	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,3))
	{
		if (keyboard_state != PRESSED_7)
		{
			keyboard_state = PRESSED_7;
			return '7';
		}	 else {
				return 0;
			}
	}
	
	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,2))
	{
		if (keyboard_state != PRESSED_STAR)
		{
			keyboard_state = PRESSED_STAR;
			return '*';
		}	 else {
				return 0;
			}
	}
	
	CLEAR_BIT(GPIO_PORTB_DATA_R, 2);
	SET_BIT(GPIO_PORTB_DATA_R, 3);
	CLEAR_BIT(GPIO_PORTB_DATA_R, 4);
	dummy = GPIO_PORTB_DATA_R;
	
	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,5))
	{
		if (keyboard_state != PRESSED_2)
		{
			keyboard_state = PRESSED_2;
			return '2';
		}	 else {
				return 0;
			}
	}

	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,4))
	{
		if (keyboard_state != PRESSED_5)
		{
			keyboard_state = PRESSED_5;
			return '5';
		}	 else {
				return 0;
			}
	}
	
	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,3))
	{
		if (keyboard_state != PRESSED_8)
		{
			keyboard_state = PRESSED_8;
			return '8';
		}	 else {
				return 0;
			}
	}
	
	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,2))
	{
		if (keyboard_state != PRESSED_0)
		{
			keyboard_state = PRESSED_0;
			return '0';
		}	 else {
				return 0;
			}
	}
	
	CLEAR_BIT(GPIO_PORTB_DATA_R, 2);
	CLEAR_BIT(GPIO_PORTB_DATA_R, 3);
	SET_BIT(GPIO_PORTB_DATA_R, 4);
	dummy = GPIO_PORTB_DATA_R;
	
	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,5))
	{
		if (keyboard_state != PRESSED_3)
		{
			keyboard_state = PRESSED_3;
			return '3';
		}	 else {
				return 0;
			}
	}

	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,4))
	{
		if (keyboard_state != PRESSED_6)
		{
			keyboard_state = PRESSED_6;
			return '6';
		} else {
			return 0;
		}
	}
	
	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,3))
	{
		if (keyboard_state != PRESSED_9)
		{
			keyboard_state = PRESSED_9;
			return '9';
		}	 else {
				return 0;
			}
	}
	
	if(TEST_BIT_HIGH(GPIO_PORTD_DATA_R,2))
	{
		if (keyboard_state != PRESSED_HASH)
		{
			keyboard_state = PRESSED_HASH;
			return '#';
		}	 else {
				return 0;
			}
	}
	
	keyboard_state = PRESSED_NONE;
	return 0;
}

void keyboard_task( void )
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
	INT8U key;
	gasstation_event event;
	
	// check for if 1,4,7 and * is pressed on num_pad
	xSemaphoreTake(lcd_keyboard_port_mutex, portMAX_DELAY );
	key = read_keyboard();
	xSemaphoreGive(lcd_keyboard_port_mutex);
	if(key != 0)
	{
		// Add key to some queue.
		//xQueueSend(input_queue,&key, 0);
		event.event = EVENT_KEYBOARD_DIGIT;
		event.value = key;
		xQueueSend(event_queue, &event, 0);
	}
}
/****************************** End Of Module *******************************/


