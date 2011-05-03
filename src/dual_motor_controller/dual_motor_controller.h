/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: dual_motor_controller.h
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

#ifndef _DUAL_MOTOR_CONTROLLER_H
  #define _DUAL_MOTOR_CONTROLLER_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

INT8U motor_get_direction( INT8U m );
/*****************************************************************************
*   Function : Gets the direction for motor m as lastly reported from the FPGA.
*****************************************************************************/

INT16U motor_get_speed( INT8U m );
/*****************************************************************************
*   Function : Gets the speed for motor m as lastly reported from the FPGA.
*****************************************************************************/

INT16U motor_get_position( INT8U m );
/*****************************************************************************
*   Function : Gets the position for motor m as lastly reported from the FPGA.
*****************************************************************************/

void dual_motor_task();
/*****************************************************************************
*   Function : Gets motorevenst from the queue and parses them.
*****************************************************************************/

void init_dual_motor_controller();
/*****************************************************************************
*   Function : Initializes the dual motors.
*****************************************************************************/

/****************************** End Of Module *******************************/
#endif
