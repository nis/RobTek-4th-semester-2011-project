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
		return motor_one_direction; 
	}
	
	if(m == MOTOR_TWO)
	{
		return motor_two_direction; 
	}
}

INT16U motor_get_speed( INT8U m )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	if(m == MOTOR_ONE)
	{
		return motor_one_speed; 
	}
	
	if(m == MOTOR_TWO)
	{
		return motor_two_speed; 
	}
}

INT16U motor_get_position( INT8U m )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	if(m == MOTOR_ONE)
	{
		return motor_one_position; 
	}
	
	if(m == MOTOR_TWO)
	{
		return motor_two_position; 
	}
}

void init_dual_motor_controller()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{

}

/****************************** End Of Module *******************************/
