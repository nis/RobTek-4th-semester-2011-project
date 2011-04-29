#include "../inc/lm3s6965.h"
#include "../inc/emp_type.h"
#include "../cpu/cpu.h"
#include "../inc/binary.h"
#include "lcd.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "../defines.h"

#define DELAY_NS(x)		(x-20)*(FCPU/1000000000)/3
#define DELAY_US(x)		(x*(FCPU/1000000))/3
#define DELAY_MS(x)		(x*(FCPU/1000))/3

void lcd_write_4bit_mode(INT8U c);
BOOLEAN display_buffer_goto_xy(INT8U col_p, INT8U row_p);

INT8U display_buffer[COL_SIZE][ROW_SIZE];
INT8U display_buffer_dirty_bits[COL_SIZE][ROW_SIZE];
INT8U col_pointer = 0;
INT8U row_pointer = 0;

INT8U int_to_ascii(INT8U number)
/*****************************************************************************
*   Function : Accepts an int < 10 and returns the ascii value for that number
*****************************************************************************/
{
	if(number < 10)
	{
		return 0x30 + number;
	} else {
		return 0x30;
	}
}

void write_2_char_int_to_buffer (INT8U start, INT8U line, INT8U i )
{
	INT8U c1 = int_to_ascii((i / 10) % 10);
	INT8U c2 = int_to_ascii((i / 1) % 10);
	
	if(c1 == 0x30)
	{
		c1 = 0x20;
	}
	
	lcd_add_char_to_buffer(start, line, c1);
	lcd_add_char_to_buffer(start + 1, line, c2);
}

void write_3_char_int_to_buffer (INT8U start, INT8U line, INT8U i )
{
	INT8U c1 = int_to_ascii((i / 100) % 10);
	INT8U c2 = int_to_ascii((i / 10) % 10);
	INT8U c3 = int_to_ascii((i / 1) % 10);
	
	if(c1 == 0x30)
	{
		c1 = 0x20;
		if(c2 == 0x30)
		{
			c2 = 0x20;
		}
	}
	
	lcd_add_char_to_buffer(start, line, c1);
	lcd_add_char_to_buffer(start + 1, line, c2);
	lcd_add_char_to_buffer(start + 2, line, c3);
}

void write_4_char_int_to_buffer (INT8U start, INT8U line, INT16U i )
{
	INT8U c1 = int_to_ascii((i / 1000) % 10);
	INT8U c2 = int_to_ascii((i / 100) % 10);
	INT8U c3 = int_to_ascii((i / 10) % 10);
	INT8U c4 = int_to_ascii((i / 1) % 10);
	
	if(c1 == 0x30)
	{
		c1 = 0x20;
		if(c2 == 0x30)
		{
			c2 = 0x20;
			if(c3 == 0x30)
			{
				c3 = 0x20;
			}
		}
	}
	
	lcd_add_char_to_buffer(start, line, c1);
	lcd_add_char_to_buffer(start + 1, line, c2);
	lcd_add_char_to_buffer(start + 2, line, c3);
	lcd_add_char_to_buffer(start + 3, line, c4);
}

void write_4_char_int_to_buffer_with_comma (INT8U start, INT8U line, INT16U i )
{
	INT8U c1 = int_to_ascii((i / 1000) % 10);
	INT8U c2 = int_to_ascii((i / 100) % 10);
	INT8U c3 = int_to_ascii((i / 10) % 10);
	INT8U c4 = int_to_ascii((i / 1) % 10);
	
	if(c1 == 0x30)
	{
		// c1 = 0x20;
		// if(c2 == 0x30)
		// {
		// 	c2 = 0x20;
		// 	if(c3 == 0x30)
		// 	{
		// 		c3 = 0x20;
		// 	}
		// }
	}
	
	lcd_add_char_to_buffer(start, line, c1);
	lcd_add_char_to_buffer(start + 1, line, c2);
	lcd_add_char_to_buffer(start + 2, line, ',');
	lcd_add_char_to_buffer(start + 3, line, c3);
	lcd_add_char_to_buffer(start + 4, line, c4);
}

void write_5_char_int_to_buffer (INT8U start, INT8U line, INT16U i )
{
	INT8U c1 = int_to_ascii((i / 10000) % 10);
	INT8U c2 = int_to_ascii((i / 1000) % 10);
	INT8U c3 = int_to_ascii((i / 100) % 10);
	INT8U c4 = int_to_ascii((i / 10) % 10);
	INT8U c5 = int_to_ascii((i / 1) % 10);
	
	if(c1 == 0x30)
	{
		c1 = 0x20;
		if(c2 == 0x30)
		{
			c2 = 0x20;
			if(c3 == 0x30)
			{
				c3 = 0x20;
				if(c4 == 0x30)
				{
					c4 = 0x20;
				}
			}
		}
	}
	
	lcd_add_char_to_buffer(start, line, c1);
	lcd_add_char_to_buffer(start + 1, line, c2);
	lcd_add_char_to_buffer(start + 2, line, c3);
	lcd_add_char_to_buffer(start + 3, line, c4);
	lcd_add_char_to_buffer(start + 4, line, c5);
}

void write_5_char_int_to_buffer_with_comma (INT8U start, INT8U line, INT16U i )
{
	INT8U c1 = int_to_ascii((i / 10000) % 10);
	INT8U c2 = int_to_ascii((i / 1000) % 10);
	INT8U c3 = int_to_ascii((i / 100) % 10);
	INT8U c4 = int_to_ascii((i / 10) % 10);
	INT8U c5 = int_to_ascii((i / 1) % 10);
	
	if(c1 == 0x30)
	{
		c1 = 0x20;
		if(c2 == 0x30)
		{
			c2 = 0x20;
			if(c3 == 0x30)
			{
				c3 = 0x20;
				if(c4 == 0x30)
				{
					c4 = 0x20;
				}
			}
		}
	}
	
	lcd_add_char_to_buffer(start, line, c1);
	lcd_add_char_to_buffer(start + 1, line, c2);
	lcd_add_char_to_buffer(start + 2, line, c3);
	lcd_add_char_to_buffer(start + 3, line, ',');
	lcd_add_char_to_buffer(start + 4, line, c4);
	lcd_add_char_to_buffer(start + 5, line, c5);
}

void write_10_char_int_to_buffer (INT8U start, INT8U line, INT32U i )
{
	INT8U c1 = int_to_ascii((i / 1000000000) % 10);
	INT8U c2 = int_to_ascii((i / 100000000) % 10);
	INT8U c3 = int_to_ascii((i / 10000000) % 10);
	INT8U c4 = int_to_ascii((i / 1000000) % 10);
	INT8U c5 = int_to_ascii((i / 100000) % 10);
	INT8U c6 = int_to_ascii((i / 10000) % 10);
	INT8U c7 = int_to_ascii((i / 1000) % 10);
	INT8U c8 = int_to_ascii((i / 100) % 10);
	INT8U c9 = int_to_ascii((i / 10) % 10);
	INT8U c10 = int_to_ascii((i / 1) % 10);
	
	if(c1 == 0x30)
	{
		c1 = 0x20;
		if(c2 == 0x30)
		{
			c2 = 0x20;
			if(c3 == 0x30)
			{
				c3 = 0x20;
				if(c4 == 0x30)
				{
					c4 = 0x20;
					if(c5 == 0x30)
					{
						c5 = 0x20;
						if(c6 == 0x30)
						{
							c6 = 0x20;
							if(c7 == 0x30)
							{
								c7 = 0x20;
								if(c8 == 0x30)
								{
									c8 = 0x20;
									if(c9 == 0x30)
									{
										c9 = 0x20;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	lcd_add_char_to_buffer(start, line, c1);
	lcd_add_char_to_buffer(start + 1, line, c2);
	lcd_add_char_to_buffer(start + 2, line, c3);
	lcd_add_char_to_buffer(start + 3, line, c4);
	lcd_add_char_to_buffer(start + 4, line, c5);
	lcd_add_char_to_buffer(start + 5, line, c6);
	lcd_add_char_to_buffer(start + 6, line, c7);
	lcd_add_char_to_buffer(start + 7, line, c8);
	lcd_add_char_to_buffer(start + 8, line, c9);
	lcd_add_char_to_buffer(start + 9, line, c10);
}

void write_10_char_int_to_buffer_with_comma (INT8U start, INT8U line, INT32U i )
{
	INT8U c1 = int_to_ascii((i / 1000000000) % 10);
	INT8U c2 = int_to_ascii((i / 100000000) % 10);
	INT8U c3 = int_to_ascii((i / 10000000) % 10);
	INT8U c4 = int_to_ascii((i / 1000000) % 10);
	INT8U c5 = int_to_ascii((i / 100000) % 10);
	INT8U c6 = int_to_ascii((i / 10000) % 10);
	INT8U c7 = int_to_ascii((i / 1000) % 10);
	INT8U c8 = int_to_ascii((i / 100) % 10);
	INT8U c9 = int_to_ascii((i / 10) % 10);
	INT8U c10 = int_to_ascii((i / 1) % 10);
	
	if(c1 == 0x30)
	{
		c1 = 0x20;
		if(c2 == 0x30)
		{
			c2 = 0x20;
			if(c3 == 0x30)
			{
				c3 = 0x20;
				if(c4 == 0x30)
				{
					c4 = 0x20;
					if(c5 == 0x30)
					{
						c5 = 0x20;
						if(c6 == 0x30)
						{
							c6 = 0x20;
							if(c7 == 0x30)
							{
								c7 = 0x20;
							}
						}
					}
				}
			}
		}
	}
	
	lcd_add_char_to_buffer(start, line, c1);
	lcd_add_char_to_buffer(start + 1, line, c2);
	lcd_add_char_to_buffer(start + 2, line, c3);
	lcd_add_char_to_buffer(start + 3, line, c4);
	lcd_add_char_to_buffer(start + 4, line, c5);
	lcd_add_char_to_buffer(start + 5, line, c6);
	lcd_add_char_to_buffer(start + 6, line, c7);
	lcd_add_char_to_buffer(start + 7, line, c8);
	lcd_add_char_to_buffer(start + 8, line, ',');
	lcd_add_char_to_buffer(start + 9, line, c9);
	lcd_add_char_to_buffer(start + 10, line, c10);
}

void init_lcd_write_task(void)
{	
	// Setup LCD display
	lcd_port_setup();
	lcd_init();
	
	// Clear the lcd-buffer
	INT8U col, row;
	for (row=0; row < ROW_SIZE; row++)
	{
		for (col=0; col < COL_SIZE; col++)
		{
			display_buffer[col][row] = ' ';
			display_buffer_dirty_bits[col][row] = 0;
		}
	}
}

void lcd_set_cursor_position(INT8U row, INT8U col)
{
	if((row >= 0 && row < ROW_SIZE) && (col >= 0 && col < COL_SIZE))
	{
		INT8U pos = 0;
		pos = row*0x40 + col;
		pos |= 0x80;
		set_rs_low();				
		lcd_write_4bit_mode(pos);
		lcd_delay_us(90);
	}
}

void lcd_write_buffer()
{
	INT8U t_row, t_col;

	for(t_row = 0; t_row < ROW_SIZE; ++t_row)
	{
		for(t_col = 0; t_col < COL_SIZE; ++t_col)
		{
			lcd_set_cursor_position(t_row, t_col);
			set_rs_high();
			lcd_data_write(display_buffer[t_col][t_row]);
			lcd_delay_us(30);
		}
	}
}

BOOLEAN lcd_function(INT8U function)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	BOOLEAN	return_value = 0;  

		set_rs_low();				// write control bytes
		lcd_write_4bit_mode(function);

	return return_value;
}

BOOLEAN display_buffer_goto_xy(INT8U col_p, INT8U row_p)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  INT8U pos;
	INT8U	return_value = 0;

	if ((col_p < COL_SIZE) && (row_p < ROW_SIZE))
	{
		pos = row_p*0x40 + col_p;
 		pos |= 0x80;
		return_value = lcd_function(pos);
  	}
	return return_value;
}

void lcd_write_task()
{	
	// Find the next dirty bit and write out the char to the LCD.
	INT8U i = 0;
	xSemaphoreTake(lcd_buffer_mutex, portMAX_DELAY );
		while ( display_buffer_dirty_bits[col_pointer][row_pointer] != 1 && i < COL_SIZE * ROW_SIZE )
		{
			if(col_pointer == COL_SIZE - 1)
			{
				if(row_pointer == ROW_SIZE - 1)
				{
					row_pointer = 0;
				} else {
					row_pointer++;
				}
				col_pointer = 0;
			} else {
				col_pointer++;
			}
			
			i++;
		}
		
		if(display_buffer_dirty_bits[col_pointer][row_pointer])
		{
			xSemaphoreTake(lcd_keyboard_port_mutex, portMAX_DELAY );
				lcd_set_cursor_position(row_pointer, col_pointer);
				set_rs_high();
				lcd_data_write(display_buffer[col_pointer][row_pointer]);
				display_buffer_dirty_bits[col_pointer][row_pointer] = 0;
			xSemaphoreGive(lcd_keyboard_port_mutex);
		}
	xSemaphoreGive(lcd_buffer_mutex);
	
	if(col_pointer == COL_SIZE - 1)
	{
		if(row_pointer == ROW_SIZE - 1)
		{
			row_pointer = 0;
		} else {
			row_pointer++;
		}
		col_pointer = 0;
	} else {
		col_pointer++;
	}
}

void lcd_add_char_to_buffer(INT8U col, INT8U row, INT8U ch)
{
	xSemaphoreTake(lcd_buffer_mutex, portMAX_DELAY );
	display_buffer[col][row] = ch;
	display_buffer_dirty_bits[col][row] = 1;
	xSemaphoreGive(lcd_buffer_mutex);
}

void lcd_add_string_to_buffer(INT8U col, INT8U row, INT8S *str)
{
	while ( *str )
	{
		xSemaphoreTake(lcd_buffer_mutex, portMAX_DELAY );
		display_buffer[col][row] = *str;
		display_buffer_dirty_bits[col][row] = 1;
		xSemaphoreGive(lcd_buffer_mutex);
		col++;
		str++;
	}
	
}

void lcd_io(void)
{
	INT8S dummy;
	// Initialize internal clock
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
	dummy = SYSCTL_RCGC2_R;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
	dummy = SYSCTL_RCGC2_R;

	// Set pins as output
	GPIO_PORTA_DIR_R |= PA6;
	GPIO_PORTA_DIR_R |= PA7;

	GPIO_PORTB_DIR_R |= PB2;
	GPIO_PORTB_DIR_R |= PB3;
	GPIO_PORTB_DIR_R |= PB4;
	GPIO_PORTB_DIR_R |= PB5;
	GPIO_PORTB_DIR_R |= PB6;

	// Data enable
	GPIO_PORTA_DEN_R |= PA6;
	GPIO_PORTA_DEN_R |= PA7;

	GPIO_PORTB_DEN_R |= PB2;
	GPIO_PORTB_DEN_R |= PB3;
	GPIO_PORTB_DEN_R |= PB4;
	GPIO_PORTB_DEN_R |= PB5;
	GPIO_PORTB_DEN_R |= PB6;

	// set RS high
	set_rs_high();
	// set RW low
	CLEAR_BIT(GPIO_PORTA_DATA_R, LCD_RW);
	// Set E low
	CLEAR_BIT(GPIO_PORTB_DATA_R, LCD_E);

}

void set_rs_high()
{
	SET_BIT(GPIO_PORTA_DATA_R, LCD_RS);
}

void set_rs_low()
{
	CLEAR_BIT(GPIO_PORTA_DATA_R, LCD_RS);
}

void lcd_e_high()
{
	// tcycle = 500 ns; // pw = 230 ns;
	SET_BIT(GPIO_PORTB_DATA_R, LCD_E);
	// 230 / 20 almost 12
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
}

void lcd_e_low()
{
	// tcycle = 500 ns; // pw = 230 ns;
	CLEAR_BIT(GPIO_PORTB_DATA_R, LCD_E);
	// 230 / 20 almost 12
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
}

void lcd_e_high_low()
{
	lcd_e_high();
	lcd_e_low();
}

void lcd_cmd_write(INT8U cmd)
{

	lcd_e_high();
	GPIO_PORTB_DATA_R &= ~(0x0F) << 2; // Clear bits (Mask)
	GPIO_PORTB_DATA_R |= (cmd & 0x0F) << 2; // Set new bits
	lcd_e_low();

	/*
	GPIO_PORTB_DATA_R = ((cmd & 0x0F)<< 2) | (GPIO_PORTB_DATA_R & ~(0x3C));
	lcd_e_high_low();
	*/
}

void lcd_data_write(INT8U data)
{
	lcd_e_high();
	GPIO_PORTB_DATA_R &= ~(0x0F) << 2; // Clear bits (Mask)
	GPIO_PORTB_DATA_R |= (data & 0xF0) >> 2; // Set high bits
	lcd_e_low();
	lcd_e_high();
	GPIO_PORTB_DATA_R &= ~(0x0F) << 2; // Clear bits (Mask)
	GPIO_PORTB_DATA_R |= (data & 0x0F) << 2; // Set low bits
	lcd_e_low();
}

void lcd_init()
{
	lcd_delay_ms(15); 		// Delay 15 ms
	set_rs_low();
	lcd_cmd_write(0x03);
	lcd_delay_ms(5);
	lcd_cmd_write(0x03);
	lcd_delay_us(100);
	lcd_cmd_write(0x03);
	lcd_delay_ms(5);

	lcd_cmd_write(0x02);	// Set 4-bit mode
	lcd_delay_us(40);
	lcd_data_write(0x28);	// 4-bit mode, 1/16 duty, 5x8 font
	lcd_delay_us(40);
	lcd_data_write(0x08); 	// display off
	lcd_delay_us(40);
	//lcd_data_write(0x0C);	// display on, blink cursor on
	lcd_data_write(0b00001110);	// display on
	lcd_delay_us(40);

	set_rs_high();
}

void lcd_delay_us(INT32U i)
{
	i = DELAY_US(i);
	__asm(	"			mov r3, %0 	\n\t"
			"uloop: 	sub r3, #1	\n\t"
			"			cmp r3, #0	\n\t"
			"			bne uloop	\n\t"
			:
			: "r" (i));
}

void lcd_delay_ms(INT32U i)
{
	i = DELAY_MS(i);
	__asm(	"			mov r3, %0 	\n\t"
			"mloop: 	sub r3, #1	\n\t"
			"			cmp r3, #0	\n\t"
			"			bne mloop	\n\t"
			:
			: "r" (i));
}

void lcd_write_port(INT8U value)
{
	GPIO_PORTB_DATA_R = ((value & 0x0F)<< 2) | (GPIO_PORTB_DATA_R & ~(0x3C));
}

void lcd_write_4bit_mode(INT8U c)
/*****************************************************************************
*   Input    :	c: character.
*   Output   :	-
*   Function : 	write a character to the LCD in 4 bit mode
******************************************************************************/
{	
	lcd_e_high();
	GPIO_PORTB_DATA_R = ((c & 0xF0)>> 2) | (GPIO_PORTB_DATA_R & ~(0x3C));
	lcd_e_low();
	lcd_e_high();
	GPIO_PORTB_DATA_R = ((c & 0x0F)<< 2) | (GPIO_PORTB_DATA_R & ~(0x3C));
	lcd_e_low();
}

void lcd_init_display(void)
/*****************************************************************************
*   Input    : 	-
*   Output   : 	-
*   Function : 	Initialise the LCD - put into 4 bit mode
******************************************************************************/
{
	lcd_delay_us(15000);
	set_rs_low();						// write control bytes
	lcd_write_port(0x03);  	// attention!
    lcd_e_high_low();
	lcd_delay_us(5000);
	lcd_e_high_low();
	lcd_delay_us(100);
	lcd_e_high_low();
	lcd_delay_us(5000);
	lcd_write_port(0x2);			// set 4 bit mode
	lcd_e_high_low();
	lcd_delay_us(40);
	lcd_write_4bit_mode(0x28);	// 4 bit mode, 1/16 duty, 5x8 font
	lcd_delay_us(40);
	lcd_write_4bit_mode(0x08);	// display off
	lcd_delay_us(40);
	lcd_write_4bit_mode(0x0C);	// display on, blink curson on
	lcd_delay_us(40);
	lcd_write_4bit_mode(0x06);	// entry mode
	lcd_delay_us(40);
	lcd_write_4bit_mode(0x01);	// Home
//	lcd_delay_us(30000);
	set_rs_high();			// write characters

}

void lcd_port_setup(void)
/*****************************************************************************
*   Input    : 	-
*   Output   : 	-
*   Function : 	LCD port setup.
******************************************************************************/
{
		INT8S dummy;

		// LCD_RS PA7
		// LCD_RW PA6
		// LCD_E  PB6 // Strobe
		// LCD_D4..D7 // PB2 .. PB5

		// Enable the GPIO port that is used for LCD_RW.
	  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
	  // Do a dummy read to insert a few cycles after enabling the peripheral.
	  dummy = SYSCTL_RCGC2_R;
	  // Set the direction as output.
	  SET_BIT(GPIO_PORTA_DIR_R, LCD_RW);
	  // Enable the GPIO pins for digital function.
	  SET_BIT(GPIO_PORTA_DEN_R, LCD_RW);
	  // Set pin low
	  CLEAR_BIT(GPIO_PORTA_DATA_R, LCD_RW);

	  // Enable the GPIO port that is used for LCD_RS.
	  // Set the direction as output.
	  SET_BIT(GPIO_PORTA_DIR_R, LCD_RS);
	  // Enable the GPIO pins for digital function.
	  SET_BIT(GPIO_PORTA_DEN_R, LCD_RS);
	  // Set pin high
	  SET_BIT(GPIO_PORTA_DATA_R, LCD_RS);

	  // Enable the GPIO port that is used for LCD_E (Strobe).
 	  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
 	  // Do a dummy read to insert a few cycles after enabling the peripheral.
 	  dummy = SYSCTL_RCGC2_R;

	  // Set the direction as output.
	  SET_BIT(GPIO_PORTB_DIR_R, LCD_E);
	  // Enable the GPIO pins for digital function.
	  SET_BIT(GPIO_PORTB_DEN_R, LCD_E);
	  // Set pin low
	  CLEAR_BIT(GPIO_PORTB_DATA_R, LCD_E);

	  // Set the direction as output for LCD_D4 .. LCD_D7
	  GPIO_PORTB_DIR_R &=  ~(0x3C);
	  GPIO_PORTB_DIR_R |= 0x3C;
	  // Enable digital function for LCD_E and LCD_D4 .. LCD_D7
	  GPIO_PORTB_DEN_R &=  ~(0x3C);
	  GPIO_PORTB_DEN_R |= 0x3C;
}
