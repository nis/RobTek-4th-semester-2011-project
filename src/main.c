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
#include "inc/binary.h"
#include "defines.h"
#include "cpu/cpu.h"
#include "led/led.h"
#include "spi/spi.h"
#include "inc/lm3s6965.h"
#include "lcd/lcd.h"
//#include "pot/pot.h"
#include "buttons/buttons.h"
#include "emp_fpga_protocol/emp_fpga_protocol.h"
#include "dual_motor_controller/dual_motor_controller.h"
#include "uart/uart.h"
#include "regulation/regulation.h"
#include "joystick/joystick.h"
 
// all mutex used in this c program
xSemaphoreHandle lcd_buffer_mutex;
xSemaphoreHandle lcd_keyboard_port_mutex;
xSemaphoreHandle motor_one_speed_mutex;
xSemaphoreHandle motor_two_speed_mutex;
xSemaphoreHandle motor_one_direction_mutex;
xSemaphoreHandle motor_two_direction_mutex;
xSemaphoreHandle motor_one_position_mutex;
xSemaphoreHandle motor_two_position_mutex;
xSemaphoreHandle y_pos_mutex;
xSemaphoreHandle x_pos_mutex;
xSemaphoreHandle y_target_pos_mutex;
xSemaphoreHandle x_target_pos_mutex;
 
// Queues
xQueueHandle motor_event_queue;
xQueueHandle motor_command_queue;
xQueueHandle spi_input_queue;
xQueueHandle spi_output_queue;
xQueueHandle ui_event_queue;
xQueueHandle uart_command_queue;
 
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
    init_uart0();
    init_joystick();
     
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
void working_task(void *pvParameters) {
    //INT8U counter = 0;
     

	lcd_add_string_to_buffer(0, 0, "X               ");
    lcd_add_string_to_buffer(0, 1, "Y               ");
    // lcd_add_string_to_buffer(0, 0, "                ");
    //     lcd_add_string_to_buffer(0, 1, "                ");
    //      
    //     motor_send_command(MOTOR_X, MOTOR_CW, 0);
    //     motor_send_command(MOTOR_Y, MOTOR_CW, 0);
     
    while (1) {
         
		// Show target positions
		write_5_char_int_to_buffer (1, 0, get_x_target_pos() );
		write_5_char_int_to_buffer (1, 1, get_y_target_pos() );
		
		// Show position from FPGA
		write_5_char_int_to_buffer (6, 0, motor_get_position( MOTOR_X ) );
		write_5_char_int_to_buffer (6, 1, motor_get_position( MOTOR_Y ) );
         
        // Events from buttons
        // if(uxQueueMessagesWaiting(ui_event_queue) != 0)
        // {
        //     status = xQueueReceive(ui_event_queue, &event, 0);
        //     
        //     if(status == pdPASS)
        //     {
        //         switch ( event.event )
        //         {
        //             case UP_CLICK:
        //             if(speed < 49)
        //             {
        //                 speed++;
        //             }
        //             break;
        //             
        //             case DOWN_CLICK:
        //             if(speed > 0)
        //             {
        //                 speed--;
        //             }
        //             break;
        //             
        //             case LEFT_CLICK:
        //             if(direction == MOTOR_CW && speed == 0)
        //             {
        //                 direction = MOTOR_CCW;
        //             }
        //             break;
        //             
        //             case RIGHT_CLICK:
        //             if(direction == MOTOR_CCW && speed == 0)
        //             {
        //                 direction = MOTOR_CW;
        //             }
        //             break;
        //         }
        //         
        //         motor_send_command(MOTOR_Y, direction, speed);
        //         
        //         if(direction == MOTOR_CW)
        //         {
        //             lcd_add_string_to_buffer(1, 0, "CW ");
        //         }
        //         
        //         if(direction == MOTOR_CCW)
        //         {
        //             lcd_add_string_to_buffer(1, 0, "CCW");
        //         }
        //         
        //         write_3_char_int_to_buffer (7, 0, speed );
        //     }
        // }
         
         
        // // Position
        // write_5_char_int_to_buffer (11, 1, motor_get_position( MOTOR_Y ) );
        //  
        // // Speed
        // write_5_char_int_to_buffer (5, 1, motor_get_speed( MOTOR_Y ) );
        //  
        // // Direction
        // if(motor_get_direction( MOTOR_Y ) == MOTOR_CW)
        // {
        //     lcd_add_string_to_buffer(1, 1, "CW ");
        // }
        //  
        // if(motor_get_direction( MOTOR_Y ) == MOTOR_CCW)
        // {
        //     lcd_add_string_to_buffer(1, 1, "CCW");
        // }
         
        vTaskDelay(10) ;
    }
}
 
/**
 * LCD task
 */
void vUserTask4(void *pvParameters) {
    
	// lcd_add_string_to_buffer(0, 0, "                ");
	// lcd_add_string_to_buffer(0, 1, "                ");

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
		
		if(TEST_BIT_HIGH(SSI0_SR_R, 3))
		{
			// Receive full
			led_red_toggle();
		}
		
		if(TEST_BIT_LOW(SSI0_SR_R, 1))
		{
			// Send full
			led_green_toggle();
		}
		
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
 * Dual motor driver.
 */
void motor_task(void *pvParameters) {
    while (1) {
        dual_motor_receive_task();
        vTaskDelay(10) ;
    }
}
 
/**
 * UART0 receive task
 */
void uart0_receive_task_runner(void *pvParameters)
{
    while (1)
    {
        uart0_receive_task();
        vTaskDelay(10);
    }
}
 
/**
 * Regulation task
 */
void regulation_task_runner(void *pvParameters)
{
    while (1)
    {
        regulation_task();
        vTaskDelay(10);
    }
}
 
/**
 * Joystick task
 */
void joystick_task_runner(void *pvParameters)
{
    while (1)
    {
        joystick_task();
        vTaskDelay(1);
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
    xTaskCreate( working_task, ( signed portCHAR * ) "Task3", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( vUserTask4, ( signed portCHAR * ) "Task4", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( spi_task, ( signed portCHAR * ) "Task5", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( protocol_task_runner, ( signed portCHAR * ) "Task6", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( motor_task, ( signed portCHAR * ) "Task7", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( uart0_receive_task_runner, ( signed portCHAR * ) "Task8", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( regulation_task_runner, ( signed portCHAR * ) "Task8", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( joystick_task_runner, ( signed portCHAR * ) "Joystick", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

     
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

	y_pos_mutex = xSemaphoreCreateMutex();
    if ( y_pos_mutex == NULL )
    {
        led_red_on();
        while(1);
    }

	x_pos_mutex = xSemaphoreCreateMutex();
    if ( x_pos_mutex == NULL )
    {
        led_red_on();
        while(1);
    }

	y_target_pos_mutex = xSemaphoreCreateMutex();
    if ( y_target_pos_mutex == NULL )
    {
        led_red_on();
        while(1);
    }

	x_target_pos_mutex = xSemaphoreCreateMutex();
    if ( x_target_pos_mutex == NULL )
    {
        led_red_on();
        while(1);
    }
     
    /* 
     * Setup queues.
     */
    motor_command_queue = xQueueCreate(16, sizeof( motor_command ) );
    if (motor_command_queue == NULL)
    {
        led_red_on();
        while(1);
    }
         
    motor_event_queue = xQueueCreate(16, sizeof( motor_event ) );
    if (motor_event_queue == NULL)
    {
        led_red_on();
        while(1);
    }
     
    spi_input_queue = xQueueCreate(16, sizeof( INT16U ) );
    if (spi_input_queue == NULL)
    {
        led_red_on();
        while(1);
    }
     
    spi_output_queue = xQueueCreate(16, sizeof( INT16U ) );
    if (spi_output_queue == NULL)
    {
        led_red_on();
        while(1);
    }
     
    ui_event_queue = xQueueCreate(16, sizeof( ui_event ) );
    if (ui_event_queue == NULL)
    {
        led_red_on();
        while(1);
    }
 
    uart_command_queue = xQueueCreate(16, sizeof( uart_command ) );
    if (uart_command_queue == NULL)
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