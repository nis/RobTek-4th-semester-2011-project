/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: dual_motor_controller.c
*
* PROJECT....: EMP
*
* DESCRIPTION:
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 110503  NS    Module created.
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

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT16U motor_one_speed = 0;
INT16U motor_two_speed = 0;

INT8U motor_one_direction = MOTOR_CW;
INT8U motor_two_direction = MOTOR_CW;

INT8U motor_one_position = 0;
INT8U motor_two_position = 0;

/*****************************   Functions   *******************************/

INT8U motor_get_direction( INT8U m )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	if(m == MOTOR_ONE)
	{
		xSemaphoreTake(motor_one_direction_mutex, portMAX_DELAY );
		return motor_one_direction;
		xSemaphoreGive(motor_one_direction_mutex);
	}

	if(m == MOTOR_TWO)
	{
		xSemaphoreTake(motor_two_direction_mutex, portMAX_DELAY );
		return motor_two_direction;
		xSemaphoreGive(motor_two_direction_mutex);
	}
}

INT16U motor_get_speed( INT8U m )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	if(m == MOTOR_ONE)
	{
		xSemaphoreTake(motor_one_speed_mutex, portMAX_DELAY );
		return motor_one_speed;
		xSemaphoreGive(motor_one_speed_mutex);
	}
	
	if(m == MOTOR_TWO)
	{
		xSemaphoreTake(motor_two_speed_mutex, portMAX_DELAY );
		return motor_two_speed;
		xSemaphoreGive(motor_two_speed_mutex);
	}
}

INT16U motor_get_position( INT8U m )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	if(m == MOTOR_ONE)
	{
		xSemaphoreTake(motor_one_position_mutex, portMAX_DELAY );
		return motor_one_position;
		xSemaphoreGive(motor_one_position_mutex);
	}

	if(m == MOTOR_TWO)
	{
		xSemaphoreTake(motor_two_position_mutex, portMAX_DELAY );
		return motor_two_position;
		xSemaphoreGive(motor_two_position_mutex);
	}
}

void dual_motor_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	
	if(uxQueueMessagesWaiting(motor_event_queue) != 0)
	{
		motor_event event;
		
		if(xQueueReceive(motor_event_queue, &event, 0) == pdPASS)
		{
			if(event.motor == MOTOR_TWO)
			{
				if(event.type == MOTOR_POS)
				{
					// Position
					write_5_char_int_to_buffer (11, 1, event.value );
				}

				if(event.type == MOTOR_SPEED)
				{
					// Speed
					write_5_char_int_to_buffer (5, 1, event.value );
					
					// Direction
					if(event.direction == MOTOR_CW)
					{
						lcd_add_string_to_buffer(1, 1, "CW ");
					}
					
					if(event.direction == MOTOR_CCW)
					{
						lcd_add_string_to_buffer(1, 1, "CCW");
					}
				}
			}
		}
	}
}

void init_dual_motor_controller()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{

}

/****************************** End Of Module *******************************/
