/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: regulation.c
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
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "../inc/emp_type.h"
#include "../defines.h"
#include "../inc/lm3s6965.h"
#include "../inc/binary.h"
#include "../dual_motor_controller/dual_motor_controller.h"
#include "../lcd/lcd.h"

/*****************************    Defines    *******************************/

// Motor/Axes
#define MOTOR_Y MOTOR_ONE
#define MOTOR_X MOTOR_TWO

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

// X-axis variables
INT16U x_current_pos;
INT8U x_current_dir = MOTOR_CW;
INT16U x_target_pos;

// X-axis variables
INT16U y_current_pos;
INT8U y_current_dir = MOTOR_CW;
INT16U y_target_pos;

/*****************************   Functions   *******************************/

void get_uart_commands(void)
/*****************************************************************************
*   Function : Parses commands from UART.
*****************************************************************************/
{
	portBASE_TYPE status;
	uart_command c;
	
	// Evenst from UART
	if(uxQueueMessagesWaiting(uart_command_queue) != 0)
	{
		status = xQueueReceive(uart_command_queue, &c, 0);
		
		if(status == pdPASS)
		{
			switch ( c.type )
			{
				case UART_COMMAND_POSITION:
					switch ( c.key )
					{
						case UART_COMMAND_X_AXIS:
						if(c.value >= 0 && c.value <= 1079)
						{
							write_5_char_int_to_buffer (11, 0, c.value ); // Disable on production
							x_target_pos = c.value;
						}
						break;
					}
				break;
			}
		}
	}
}

void regulate(void)
/*****************************************************************************
*   Function : The actual calculation of regulation.
*****************************************************************************/
{
	static INT8U pos_init = 0;
	INT16U x_delta_pos, y_delta_pos = 0;
	INT8U x_new_dir, y_new_dir = MOTOR_CW;
	INT8U x_new_speed, y_new_speed = 0;
	
	x_current_pos = motor_get_position(MOTOR_X);
	y_current_pos = motor_get_position(MOTOR_Y);
	
	if(pos_init == 0)
	{
		x_target_pos = x_current_pos;
		y_target_pos = y_current_pos;
		pos_init = 1;
	}
	
	// x-axis
	if(x_current_pos > x_target_pos)
	{
		x_delta_pos = x_current_pos - x_target_pos;
		x_new_dir = MOTOR_CW;
	} else {
		x_delta_pos = x_target_pos - x_current_pos;
		x_new_dir = MOTOR_CW;
	}
	
	if(x_delta_pos < 10)
	{
		x_delta_pos = 0;
	} else if(x_delta_pos > 99)
	{
		x_delta_pos = 49;
	}
	
	x_new_speed = x_delta_pos / 2;
	
	motor_send_command(MOTOR_X, x_new_dir, x_new_speed);
}

void regulation_task(void)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// See if there is any commands from UART to act on.
	get_uart_commands();
	
	// Actually regulate
	regulate();
}

void init_regulation()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Doesn't do anything yet.
}

/****************************** End Of Module *******************************/
