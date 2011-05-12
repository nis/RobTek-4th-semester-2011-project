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
	// UI events
		// Buttons
		#define UP_CLICK 		0
		#define DOWN_CLICK 		1
		#define LEFT_CLICK 		2
		#define RIGHT_CLICK 	3
		#define SELECT_CLICK 	4
		
	// Motor events
		// Motors
		#define MOTOR_X 0
		#define MOTOR_Y 1
		
		// Event types
		#define MOTOR_POS 1
		#define MOTOR_SPEED 0
		
		// Direction
		#define MOTOR_CW 0
		#define MOTOR_CCW 1
		
	// UART commands
		// Types
		#define UART_COMMAND_POSITION 0
		
		// Keys
		#define UART_COMMAND_X_AXIS 0
		#define UART_COMMAND_Y_AXIS 1
		

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

// Structures
typedef struct {INT8U type; INT8U motor; INT16U value; INT8U direction;} motor_event;
typedef struct {INT8U motor; INT8U direction; INT16U speed;} motor_command;
typedef struct {INT8U event; INT16U key; INT16U value;} ui_event;
typedef struct {INT8U type; INT8U key; INT16U value;} uart_command;

// all mutex used in this c program
extern xSemaphoreHandle lcd_buffer_mutex;
extern xSemaphoreHandle lcd_keyboard_port_mutex;
extern xSemaphoreHandle motor_one_speed_mutex;
extern xSemaphoreHandle motor_two_speed_mutex;
extern xSemaphoreHandle motor_one_direction_mutex;
extern xSemaphoreHandle motor_two_direction_mutex;
extern xSemaphoreHandle motor_one_position_mutex;
extern xSemaphoreHandle motor_two_position_mutex;
extern xSemaphoreHandle y_pos_mutex;
extern xSemaphoreHandle x_pos_mutex;
extern xSemaphoreHandle y_target_pos_mutex;
extern xSemaphoreHandle x_target_pos_mutex;

// all queues in this c code
extern xQueueHandle motor_event_queue;
extern xQueueHandle motor_command_queue;
extern xQueueHandle spi_input_queue;
extern xQueueHandle spi_output_queue;
extern xQueueHandle ui_event_queue;
extern xQueueHandle uart_command_queue;

/*****************************   Functions   *******************************/

/****************************** End Of Module *******************************/
#endif
