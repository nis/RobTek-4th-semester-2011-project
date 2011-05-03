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

#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "../inc/emp_type.h"
#include "../defines.h"
#include "../inc/lm3s6965.h"
#include "../inc/binary.h"
#include "../lcd/lcd.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void protocol_send_task (void)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	if(uxQueueMessagesWaiting(motor_command_queue) != 0)
	{
		motor_command command;
		INT16U package;
		
		xQueueReceive(motor_command_queue, &command, 0);
		
		if(command.direction == MOTOR_CW)
		{
			package = 50 - command.speed;
		} else {
			package = command.speed + 50;
		}
		
		write_5_char_int_to_buffer (11, 0, package );
		
		if(command.motor == MOTOR_RIGHT)
		{
			SET_BIT(package, 14);
		} else {
			CLEAR_BIT(package, 14);
		}
		SET_BIT(package, 15);
		CLEAR_BIT(event.value, 13);
		CLEAR_BIT(event.value, 12);
		CLEAR_BIT(event.value, 11);
		CLEAR_BIT(event.value, 10);
		CLEAR_BIT(event.value, 9);
		CLEAR_BIT(event.value, 8);
		CLEAR_BIT(event.value, 7);
		
		
		xQueueSend(spi_output_queue, &package, 0);
	}
}

void protocol_receive_task (void)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	
	if(uxQueueMessagesWaiting(spi_input_queue) != 0)
	{
		
		motor_event event;
		
		xQueueReceive(spi_input_queue, &event.value, 0);

		// Decipher and send to queue

		if(TEST_BIT_HIGH(event.value, 15))
		{
			event.motor = MOTOR_RIGHT;
		} else {
			event.motor = MOTOR_LEFT;
		}

		if(TEST_BIT_HIGH(event.value, 14))
		{
			event.type = MOTOR_POS;
		} else {
			event.type = MOTOR_SPEED;
			if(TEST_BIT_HIGH(event.value, 13))
			{
				event.direction = MOTOR_CCW;
			} else {
				event.direction = MOTOR_CW;
			}
		}

		CLEAR_BIT(event.value, 11);
		CLEAR_BIT(event.value, 12);
		CLEAR_BIT(event.value, 13);
		CLEAR_BIT(event.value, 14);
		CLEAR_BIT(event.value, 15);

		xQueueSend(motor_event_queue, &event, 0);
	}
}

void init_protocol()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{

}

/****************************** End Of Module *******************************/
