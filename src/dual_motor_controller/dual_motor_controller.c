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

INT16U motor_one_position = 0;
INT16U motor_two_position = 0;

/*****************************   Functions   *******************************/

void set_direction( INT8U m, INT8U d )
/*****************************************************************************
*   Function : Internal, sets direction.
*****************************************************************************/
{
	if(m == MOTOR_X)
	{
		xSemaphoreTake(motor_one_direction_mutex, portMAX_DELAY );
		motor_one_direction = d;
		xSemaphoreGive(motor_one_direction_mutex);
	}

	if(m == MOTOR_Y)
	{
		xSemaphoreTake(motor_two_direction_mutex, portMAX_DELAY );
		motor_two_direction = d;
		xSemaphoreGive(motor_two_direction_mutex);
	}
}

void set_speed( INT8U m, INT16U s )
/*****************************************************************************
*   Function : Internal, sets speed.
*****************************************************************************/
{
	if(m == MOTOR_X)
	{
		xSemaphoreTake(motor_one_speed_mutex, portMAX_DELAY );
		motor_one_speed = s;
		xSemaphoreGive(motor_one_speed_mutex);
	}
	
	if(m == MOTOR_Y)
	{
		xSemaphoreTake(motor_two_speed_mutex, portMAX_DELAY );
		motor_two_speed = s;
		xSemaphoreGive(motor_two_speed_mutex);
	}
}

void set_position( INT8U m, INT16U p )
/*****************************************************************************
*   Function : Internal, sets position.
*****************************************************************************/
{
	if(m == MOTOR_X)
	{
		xSemaphoreTake(motor_one_position_mutex, portMAX_DELAY );
		motor_one_position = p;
		xSemaphoreGive(motor_one_position_mutex);
	}

	if(m == MOTOR_Y)
	{
		xSemaphoreTake(motor_two_position_mutex, portMAX_DELAY );
		motor_two_position = p;
		xSemaphoreGive(motor_two_position_mutex);
	}
}

INT8U motor_get_direction( INT8U m )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U return_value = MOTOR_CW;
	
	if(m == MOTOR_X)
	{
		xSemaphoreTake(motor_one_direction_mutex, portMAX_DELAY );
		return_value = motor_one_direction;
		xSemaphoreGive(motor_one_direction_mutex);
	}

	if(m == MOTOR_Y)
	{
		xSemaphoreTake(motor_two_direction_mutex, portMAX_DELAY );
		return_value = motor_two_direction;
		xSemaphoreGive(motor_two_direction_mutex);
	}
	return return_value;
}

INT16U motor_get_speed( INT8U m )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT16U return_value = 0;
	if(m == MOTOR_X)
	{
		xSemaphoreTake(motor_one_speed_mutex, portMAX_DELAY );
		return_value = motor_one_speed;
		xSemaphoreGive(motor_one_speed_mutex);
	}
	
	if(m == MOTOR_Y)
	{
		xSemaphoreTake(motor_two_speed_mutex, portMAX_DELAY );
		return_value = motor_two_speed;
		xSemaphoreGive(motor_two_speed_mutex);
	}
	return return_value;
}

INT16U motor_get_position( INT8U m )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT16U return_value = 0;
	if(m == MOTOR_X)
	{
		xSemaphoreTake(motor_one_position_mutex, portMAX_DELAY );
		return_value = motor_one_position;
		xSemaphoreGive(motor_one_position_mutex);
	}

	if(m == MOTOR_Y)
	{
		xSemaphoreTake(motor_two_position_mutex, portMAX_DELAY );
		return_value = motor_two_position;
		xSemaphoreGive(motor_two_position_mutex);
	}
	return return_value;
}

void motor_new_command(INT8U m, INT16S s)
{
	motor_command c;
	c.motor = m;
	
	if(s > 500)
	{
		s = 500;
	}
	
	if(s < -500)
	{
		s = -500;
	}
	
	if(s < 0)
	{
		c.direction = MOTOR_CW;
		c.speed = s * -1;
	} else {
		c.direction = MOTOR_CCW;
		c.speed = s;
	}
	xQueueSend(motor_command_queue, &c, 0);
}

void motor_send_command(INT8U m, INT8U d, INT16U s)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	motor_command c;
	c.motor = m;
	c.direction = d;
	c.speed = s;
	xQueueSend(motor_command_queue, &c, 0);
}

void dual_motor_receive_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U i = uxQueueMessagesWaiting(motor_event_queue);
	
	if(i != 0)
	{
		while (i > 0)
		{
			motor_event event;

			if(xQueueReceive(motor_event_queue, &event, 0) == pdPASS)
			{
				if(event.type == MOTOR_POS)
				{
					// Position
					set_position(event.motor, event.value);
				}

				if(event.type == MOTOR_SPEED)
				{
					// Speed
					set_speed(event.motor, event.value);

					// Direction
					set_direction(event.motor, event.direction);
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
