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
#define MOTOR_Y MOTOR_ONE
#define MOTOR_X MOTOR_TWO

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

INT16S PIDcal ( INT8U epsilon, INT8U dt, INT16S max, INT16S min, INT16U Kp, INT16U Kd, INT16U Ki, INT16U target, INT16U current )
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
	static INT8S old_speed = 0;
	INT16U cpos =  motor_get_position(MOTOR_X);
	//INT8U epsilon, INT8U dt, INT16S max, INT16S min, INT16U Kp, INT16U Kd, INT16U Ki, INT16U target, INT16U current )
	INT16S new_speed = PIDcal ( 0, 1, 500, -500, 3, 10, 0, x_target_pos, cpos );
	motor_new_command(MOTOR_X, new_speed);
	write_5_char_int_to_buffer (11, 0, x_target_pos );
	
	// Write speed
	if(new_speed < 0)
	{
		lcd_add_string_to_buffer(5, 0, "-");
	} else {
		lcd_add_string_to_buffer(5, 0, " ");
	}
	
	write_3_char_int_to_buffer (7, 0, abs(new_speed) );
	
	// Write direction
	if(new_speed < 0)
	{
		lcd_add_string_to_buffer(1, 0, "CW ");
	} else {
		lcd_add_string_to_buffer(1, 0, "CCW ");
	}
	
	// Uart log
	// if(new_speed != old_speed)
	// {
	// 	if(new_speed < 0)
	// 	{
	// 		uart_write_ch('-');
	// 	}
	// 	uart_write_ch((abs(new_speed) / 10) + 0x30);
	// 	uart_write_ch((abs(new_speed) % 10) + 0x30);
	// 	uart_write_ch(',');
	// 	uart_write_ch(' ');
	// }
	
	old_speed = new_speed;
	
	// static INT8U pos_init = 0;
	// static INT8U x_old_speed = 0;
	// INT16S x_delta_pos, y_delta_pos, x_old_delta_pos, y_old_delta_pos = 0;
	// INT8U x_new_dir, y_new_dir = MOTOR_CW;
	// INT16S x_new_speed, y_new_speed = 0;
	// 
	// x_current_pos = motor_get_position(MOTOR_X);
	// y_current_pos = motor_get_position(MOTOR_Y);
	// 
	// if(pos_init == 0)
	// {
	// 	x_target_pos = x_current_pos;
	// 	y_target_pos = y_current_pos;
	// 	pos_init = 1;
	// }
	// 
	// // x-axis
	// // if(x_current_pos > x_target_pos)
	// // {
	// // 	x_delta_pos = x_current_pos - x_target_pos;
	// // 	//x_new_dir = MOTOR_CW;
	// // } else {
	//  	x_delta_pos = x_target_pos - x_current_pos;
	// // 	//x_new_dir = MOTOR_CCW;
	// // }
	// 
	// INT16S derivative;
	// 
	// if(x_delta_pos > 49)
	// {
	// 	x_delta_pos = 49;
	// } else if(x_delta_pos < -49)
	// {
	// 	x_delta_pos = -49;
	// }
	// 
	// 
	// 
	// //derivative = (x_delta_pos - x_old_delta_pos)/dt;
	// 
	// 
	// 
	// //x_new_speed = (Kd * derivative + x_delta_pos) / 2;
	// 
	// if(x_new_speed < 0)
	// {
	// 	x_new_dir = MOTOR_CCW;
	// 	x_new_speed = x_new_speed * -1;
	// } else {
	// 	x_new_dir = MOTOR_CW;
	// }
	// 
	// if(x_new_speed > 29)
	// {
	// 	x_new_speed = 29;
	// }
	// 
	// 
	// 
	// motor_send_command(MOTOR_X, x_new_dir, x_new_speed);
	// 
	// 
	// if(x_old_speed != x_new_speed)
	// {
	// 	if(TEST_BIT_HIGH(x_delta_pos, 15))
	// 	{
	// 		uart_write_ch('-');
	// 		CLEAR_BIT(x_delta_pos, 15);
	// 	}
	// 	uart_write_ch((x_delta_pos / 10) + 0x30);
	// 	uart_write_ch((x_delta_pos % 10) + 0x30);
	// 	uart_write_ch('|');
	// 	uart_write_ch(' ');
	// 	uart_write_ch((x_new_speed / 10) + 0x30);
	// 	uart_write_ch((x_new_speed % 10) + 0x30);
	// 	uart_write_ch(',');
	// 	uart_write_ch(' ');
	// }
	// 
	// x_old_speed = x_new_speed;
	// 
	// 
	// 
	// 
	// 
	// 
	// if(x_new_dir == MOTOR_CW)
	// {
	// 	lcd_add_string_to_buffer(1, 0, "CW ");
	// }
	// 
	// if(x_new_dir == MOTOR_CCW)
	// {
	// 	lcd_add_string_to_buffer(1, 0, "CCW");
	// }
	// 
	// write_3_char_int_to_buffer (7, 0, x_new_speed ); // Disable on production
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
