/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: uart.h
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

#ifndef _UART_H
  #define _UART_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void uart_send_10_digit_int_with_comma (INT32U i );
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/

void uart_send_10_digit_int ( INT32U i );
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/

void uart_send_4_digit_int ( INT16U i );
/*****************************************************************************
*   Function : Sends a 4-digit int as ASCII.
*****************************************************************************/

void uart0_receive_task();
/*****************************************************************************
*   Function : Task for receiving UART commands.
*****************************************************************************/

void uart0_send_task();
/*****************************************************************************
*   Function : Task for sending UART commands.
*****************************************************************************/

void uart_send_newline( void );
/*****************************************************************************
*   Function : Send the newline characters via UART0.
*****************************************************************************/

void uart_send_char(INT8U c);
/*****************************************************************************
*   Function : Sends a char over UART0.
*****************************************************************************/

void uart_send_string(INT8S *str);
/*****************************************************************************
*   Function : Sends a string over UART0.
*****************************************************************************/

void uart0_write_task(void);
/*****************************************************************************
*   Function : Task to run for sending commands from the board via UART0.
*****************************************************************************/

void init_uart0(void);
/*****************************************************************************
*   Function : Initalizes UART 0.
*****************************************************************************/

/****************************** End Of Module *******************************/
#endif
