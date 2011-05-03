/**
 * Name        : main.c
 * Version     :
 * Description : main definition for FreeRTOS application
 */

/*
 * FreeRTOS includes
 */
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "inc/emp_type.h"
#include "defines.h"
#include "cpu/cpu.h"
#include "led/led.h"
#include "spi/spi.h"
#include "inc/lm3s6965.h"
#include "lcd/lcd.h"
//#include "pot/pot.h"
#include "buttons/buttons.h"
#include "emp_fpga_protocol/emp_fpga_protocol.h"

// all mutex used in this c program
xSemaphoreHandle lcd_buffer_mutex;
xSemaphoreHandle lcd_keyboard_port_mutex;
xSemaphoreHandle motor_one_speed_mutex;
xSemaphoreHandle motor_two_speed_mutex;
xSemaphoreHandle motor_one_direction_mutex;
xSemaphoreHandle motor_two_direction_mutex;
xSemaphoreHandle motor_one_position_mutex;
xSemaphoreHandle motor_two_position_mutex;

// Queues
xQueueHandle motor_event_queue;
xQueueHandle motor_command_queue;
xQueueHandle spi_input_queue;
xQueueHandle spi_output_queue;

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE

void __error__(char *pcFilename, unsigned long ulLine) {
}

static void setupHardware(void) {
	// TODO: Put hardware configuration and initialisation in here
	disable_global_int();
	clk_system_init();
	init_leds();
	//init_pot();
	init_buttons();
	init_spi();
	init_lcd_write_task();
	
	enable_global_int();
	

	// Warning: If you do not initialize the hardware clock, the timings will be inaccurate
}

/**
 * Simple task that just toggles between to states
 */
void vUserTask1(void *pvParameters) {

	while (1) {
		button_task();
		vTaskDelay(20);
	}
}

/**
 * Alive task. Blinks the status LED 4 at 4Hz.
 */
void vUserTask2(void *pvParameters) {
	while (1) {
		led_status_toggle();
		vTaskDelay(250) ;
	}
}

/**
 * Counter sent out via SPI.
 */
void vUserTask3(void *pvParameters) {
	//INT8U counter = 0;
	
	lcd_add_string_to_buffer(0, 0, "                ");
	lcd_add_string_to_buffer(0, 1, "                ");
	
	INT8U speed = 0;
	INT8U direction = MOTOR_CW;
	INT8U cdir = 1;
	motor_command command;
	
	command.motor = MOTOR_TWO;
	command.direction = direction;
	command.speed = speed;
	xQueueSend(motor_command_queue, &command, 0);
	
	while (1) {
		
		INT8U up_clicks = get_up_clicks();
		
		if(up_clicks)
		{
			//typedef struct {INT8U motor; INT8U direction; INT8U speed} motor_command;
			
			if( cdir == 1)
			{
				if(speed >= 49)
				{
					cdir = 0;
				} else {
					speed++;
				}
			} else {
				if(speed == 0)
				{
					cdir = 1;
					if(direction == MOTOR_CW)
					{
						direction = MOTOR_CCW;
					} else {
						direction = MOTOR_CW;
					}
				} else {
					speed--;
				}
			}
			
			command.motor = MOTOR_TWO;
			command.direction = direction;
			command.speed = speed;
			xQueueSend(motor_command_queue, &command, 0);
			
			if(direction == MOTOR_CW)
			{
				lcd_add_string_to_buffer(1, 0, "CW ");
			}

			if(direction == MOTOR_CCW)
			{
				lcd_add_string_to_buffer(1, 0, "CCW");
			}

			write_3_char_int_to_buffer (7, 0, speed );
		}
		
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
		
		vTaskDelay(10) ;
	}
}

/**
 * LCD task
 */
void vUserTask4(void *pvParameters) {
	
	while (1) {
		lcd_write_task();
		
		vTaskDelay(10) ;
	}
}

/**
 * SPI task
 */
void spi_task(void *pvParameters) {
	
	while (1) {
		spi_send_task();
		spi_receive_task();
		
		vTaskDelay(10);
	}
}

/**
 * Protocol task
 */
void protocol_task_runner(void *pvParameters) {
	
	while (1) {
		protocol_receive_task();
		protocol_send_task();
		vTaskDelay(10) ;
	}
}


/**
 * Program entry point 
 */
int main(void) {
	setupHardware();

	/* 
	 * Start the tasks defined within this file/specific to this demo. 
	 */
	xTaskCreate( vUserTask1, ( signed portCHAR * ) "Task1", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vUserTask2, ( signed portCHAR * ) "Task2", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vUserTask3, ( signed portCHAR * ) "Task3", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vUserTask4, ( signed portCHAR * ) "Task4", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( spi_task, ( signed portCHAR * ) "Task5", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( protocol_task_runner, ( signed portCHAR * ) "Task6", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	
	/* 
	 * Setup semaphores.
	 */
	lcd_buffer_mutex = xSemaphoreCreateMutex();
	if ( lcd_buffer_mutex == NULL )
	{
		led_red_on();
		while(1);
	}
	
	lcd_keyboard_port_mutex = xSemaphoreCreateMutex();
	if ( lcd_keyboard_port_mutex == NULL )
	{
		led_red_on();
		while(1);
	}
	
	motor_one_speed_mutex = xSemaphoreCreateMutex();
	if ( motor_one_speed_mutex == NULL )
	{
		led_red_on();
		while(1);
	}
	
	motor_two_speed_mutex = xSemaphoreCreateMutex();
	if ( motor_two_speed_mutex == NULL )
	{
		led_red_on();
		while(1);
	}
	
	motor_one_direction_mutex = xSemaphoreCreateMutex();
	if ( motor_one_direction_mutex == NULL )
	{
		led_red_on();
		while(1);
	}
	
	motor_two_direction_mutex = xSemaphoreCreateMutex();
	if ( motor_two_direction_mutex == NULL )
	{
		led_red_on();
		while(1);
	}
	
	motor_one_position_mutex = xSemaphoreCreateMutex();
	if ( motor_one_position_mutex == NULL )
	{
		led_red_on();
		while(1);
	}
	
	motor_two_position_mutex = xSemaphoreCreateMutex();
	if ( motor_two_position_mutex == NULL )
	{
		led_red_on();
		while(1);
	}
	
	/* 
	 * Setup queues.
	 */
	motor_command_queue = xQueueCreate(128, sizeof( motor_command ) );
	if (motor_command_queue == NULL)
	{
		led_red_on();
		while(1);
	}
		
	motor_event_queue = xQueueCreate(128, sizeof( motor_event ) );
	if (motor_event_queue == NULL)
	{
		led_red_on();
		while(1);
	}
	
	spi_input_queue = xQueueCreate(128, sizeof( INT16U ) );
	if (spi_input_queue == NULL)
	{
		led_red_on();
		while(1);
	}
	
	spi_output_queue = xQueueCreate(128, sizeof( INT16U ) );
	if (spi_output_queue == NULL)
	{
		led_red_on();
		while(1);
	}
	
	/* 
	 * Start the scheduler. 
	 */
	vTaskStartScheduler();

	/* 
	 * Will only get here if there was insufficient memory to create the idle task. 
	 */
	return 1;
}
