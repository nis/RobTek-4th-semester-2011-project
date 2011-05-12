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
#include "../uart/uart.h"

/*****************************    Defines    *******************************/

// Motor/Axes
#define MOTOR_Y MOTOR_TWO
#define MOTOR_X MOTOR_ONE

// PID defines
// #define dt  1
// #define Kd  1

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

void set_x_target( INT16U t )
/*****************************************************************************
*   Function : Parses commands from UART.
*****************************************************************************/
{
	xSemaphoreTake(x_pos_mutex, portMAX_DELAY );
	x_target_pos = t;
	xSemaphoreGive(x_pos_mutex );
}

void set_y_target( INT16U t )
/*****************************************************************************
*   Function : Parses commands from UART.
*****************************************************************************/
{
	xSemaphoreTake(y_pos_mutex, portMAX_DELAY );
	y_target_pos = t;
	xSemaphoreGive(y_pos_mutex );
}

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

INT16U abs(INT16S n)
{
	INT16U return_value;
	
	if(n < 0)
	{
		return_value = n * -1;
	} else {
		return_value =  n;
	}
	
	return return_value;
}

INT16S y_PIDcal ( INT8U epsilon, INT8U dt, INT16S max, INT16S min, INT16U Kp, INT16U Kd, INT16U Ki, INT16U target, INT16U current )
{
	static INT16S pre_error = 0;
	static INT8S pre_output = 0;
	static INT16S integral = 0;
	
	INT16S error;
	INT16S derivative;
	INT16S output;
	
	error = target - current;
	
	// Error saturation
	if(error > 250)
	{
		error = 250;
	} else if(error < -250)
	{
		error = -250;
	}
	
	if(abs(error) > epsilon)
	{
		integral = integral + error*dt;
	}
	
	derivative = (error - pre_error)/dt;
	
	output = Kp*error + Ki*integral + Kd*derivative;
	
	//Saturation Filter
	if(output > max)
	{
		output = max;
	}
	else if(output < min)
	{
		output = min;
	}
 	
	// if(output != pre_output)
	// {
	// 	uart_write_ch('(');
	//     if(error < 0)
	//     {
	//     	uart_write_ch('-');
	//     }
	// 	uart_write_ch((abs(error) / 10) + 0x30);
	// 	uart_write_ch((abs(error) % 10) + 0x30);
	// 	uart_write_ch(')');
	// 	uart_write_ch(' ');
	// 	
	// }

	
	//Update error
    pre_output = output;
    pre_error = error;

	return output;
}

INT16S x_PIDcal ( INT8U epsilon, INT8U dt, INT16S max, INT16S min, INT16U Kp, INT16U Kd, INT16U Ki, INT16U target, INT16U current )
{
	static INT16S pre_error = 0;
	static INT8S pre_output = 0;
	static INT16S integral = 0;
	
	INT16S error;
	INT16S derivative;
	INT16S output;
	
	error = target - current;
	
	// Error saturation
	if(error > 250)
	{
		error = 250;
	} else if(error < -250)
	{
		error = -250;
	}
	
	if(abs(error) > epsilon)
	{
		integral = integral + error*dt;
	}
	
	derivative = (error - pre_error)/dt;
	
	output = Kp*error + Ki*integral + Kd*derivative;
	
	//Saturation Filter
	if(output > max)
	{
		output = max;
	}
	else if(output < min)
	{
		output = min;
	}
 	
	// if(output != pre_output)
	// {
	// 	uart_write_ch('(');
	//     if(error < 0)
	//     {
	//     	uart_write_ch('-');
	//     }
	// 	uart_write_ch((abs(error) / 10) + 0x30);
	// 	uart_write_ch((abs(error) % 10) + 0x30);
	// 	uart_write_ch(')');
	// 	uart_write_ch(' ');
	// 	
	// }

	
	//Update error
    pre_output = output;
    pre_error = error;

	return output;
}

void regulate(void)
/*****************************************************************************
*   Function : The actual calculation of regulation.
*****************************************************************************/
{
	//INT8U epsilon, INT8U dt, INT16S max, INT16S min, INT16U Kp, INT16U Kd, INT16U Ki, INT16U target, INT16U current )
	
	// x-axis regulation
	xSemaphoreTake(x_pos_mutex, portMAX_DELAY );
	INT16S x_new_speed = x_PIDcal ( 0, 1, 500, -500, X_P, X_D, X_I, get_x_target_pos(), motor_get_position(MOTOR_X) );
	xSemaphoreGive(x_pos_mutex );
	motor_new_command(MOTOR_X, x_new_speed);
	
	// y-axis regulation
	xSemaphoreTake(y_pos_mutex, portMAX_DELAY );
	INT16S y_new_speed = y_PIDcal ( 0, 1, 500, -500, Y_P, Y_D, Y_I, get_y_target_pos(), motor_get_position(MOTOR_Y) );
	xSemaphoreGive(y_pos_mutex );
	//motor_new_command(MOTOR_Y, y_new_speed);
	motor_new_command(MOTOR_Y, y_new_speed);
	
	// // Information
	// write_5_char_int_to_buffer (11, 0, x_target_pos );
	// 
	// // Write speed
	// if(x_new_speed < 0)
	// {
	// 	lcd_add_string_to_buffer(5, 0, "-");
	// } else {
	// 	lcd_add_string_to_buffer(5, 0, " ");
	// }
	// 
	// write_3_char_int_to_buffer (7, 0, abs(x_new_speed) );
	// 
	// // Write direction
	// if(x_new_speed < 0)
	// {
	// 	lcd_add_string_to_buffer(1, 0, "CW ");
	// } else {
	// 	lcd_add_string_to_buffer(1, 0, "CCW ");
	// }
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
