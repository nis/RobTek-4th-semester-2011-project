/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: defines.h
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
* 050128  NS    Module created.
*
*****************************************************************************/

#ifndef _DEFINES_H
  #define _DEFINES_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

	// Motor events
		// Motors
		#define MOTOR_ONE 0
		#define MOTOR_TWO 1
		
		// Event types
		#define MOTOR_POS 1
		#define MOTOR_SPEED 0
		
		// Direction
		#define MOTOR_CW 0
		#define MOTOR_CCW 1
		
		

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

// Structures
typedef struct {INT8U type; INT8U motor; INT16U value; INT8U direction;} motor_event;
typedef struct {INT8U motor; INT8U direction; INT8U speed} motor_command;

// all mutex used in this c program
extern xSemaphoreHandle lcd_buffer_mutex;
extern xSemaphoreHandle lcd_keyboard_port_mutex;

// all queues in this c code
extern xQueueHandle motor_event_queue;
extern xQueueHandle motor_command_queue;
extern xQueueHandle spi_input_queue;
extern xQueueHandle spi_output_queue;

/*****************************   Functions   *******************************/

/****************************** End Of Module *******************************/
#endif
