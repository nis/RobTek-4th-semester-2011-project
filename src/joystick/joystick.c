/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: joystick.c
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
* 050128  KA    Module created.
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
#include "../lcd/lcd.h"

/*****************************    Defines    *******************************/

#define BUFFER_SIZE 5

// Hardware defines
#define AXIS_START_LIMITS 20
#define AXIS_DEADBAND 15

// Virtual 2D space
#define VIRTUAL_SPACE_DIMENSIONS 1079
#define VIRTUAL_SPACE_SCALE_FACTOR 150

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

// Virtual space variables
INT32U x_v_pos = VIRTUAL_SPACE_DIMENSIONS * VIRTUAL_SPACE_SCALE_FACTOR / 2;
INT32U y_v_pos = VIRTUAL_SPACE_DIMENSIONS * VIRTUAL_SPACE_SCALE_FACTOR / 2;

// Joystick variables
INT16U x_current = 0;
INT16U y_current = 0;

INT16U x_values[BUFFER_SIZE];
INT8U x_pointer = 0;

INT16U y_values[BUFFER_SIZE];
INT8U y_pointer = 0;

INT16S x_speed_lower_limit = -AXIS_START_LIMITS;
INT16S x_speed_upper_limit = AXIS_START_LIMITS;
INT8U x_deadband = AXIS_DEADBAND;
INT8U x_pos_stepsize, x_neg_stepsize = 0;

INT16S y_speed_lower_limit = -AXIS_START_LIMITS;
INT16S y_speed_upper_limit = AXIS_START_LIMITS;
INT8U y_deadband = AXIS_DEADBAND;
INT8U y_pos_stepsize, y_neg_stepsize = 0;

INT16S x_speed = 0;
INT16S y_speed = 0;
INT8S x_rel_speed = 0;
INT8S y_rel_speed = 0;

INT8U x_initialized = 0;
INT8U y_initialized = 0;

/*****************************   Functions   *******************************/

INT16U jabs(INT16S n)
/*****************************************************************************
*   Function : Calculates absolute value.
*****************************************************************************/
{
	INT16U return_value;
	
	if(n < 0)
	{
		return_value = n * -1;
	} else {
		return_value =  n;
	}
	
	return return_value;
}

void calculate_v_space ( void )
/*****************************************************************************
*   Function : Calculates the virtual pointer in 2D space.
*****************************************************************************/
{
	// Calculate x-axis position in virtual space
	if(x_rel_speed < 0)
	{
		if(jabs(x_rel_speed) > x_v_pos)
		{
			x_v_pos = 0;
		} else {
			x_v_pos += x_rel_speed;
		}
	} else {
		if((VIRTUAL_SPACE_DIMENSIONS * VIRTUAL_SPACE_SCALE_FACTOR - x_v_pos) < x_rel_speed)
		{
			x_v_pos = VIRTUAL_SPACE_DIMENSIONS * VIRTUAL_SPACE_SCALE_FACTOR;
		} else {
			x_v_pos += x_rel_speed;
		}
	}
	
	// Calculate y-axis position in virtual space
	if(y_rel_speed < 0)
	{
		if(jabs(y_rel_speed) > y_v_pos)
		{
			y_v_pos = 0;
		} else {
			y_v_pos += y_rel_speed;
		}
	} else {
		if((VIRTUAL_SPACE_DIMENSIONS * VIRTUAL_SPACE_SCALE_FACTOR - y_v_pos) < y_rel_speed)
		{
			y_v_pos = VIRTUAL_SPACE_DIMENSIONS * VIRTUAL_SPACE_SCALE_FACTOR;
		} else {
			y_v_pos += y_rel_speed;
		}
	}
	
	// Write to LCD
	write_5_char_int_to_buffer (10, 0, (x_v_pos / VIRTUAL_SPACE_SCALE_FACTOR) );
	write_5_char_int_to_buffer (10, 1, (y_v_pos / VIRTUAL_SPACE_SCALE_FACTOR) );
}

INT8S clean_y_speed ( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT16S o = y_speed;
	
	// Calculate stepsize
	if(y_neg_stepsize == 0)
	{
		y_neg_stepsize = jabs(y_speed_lower_limit) / 100;
	}
	
	if(y_pos_stepsize == 0)
	{
		y_pos_stepsize = y_speed_upper_limit / 100;
	}
	
	// Calculate stepsize if new limits
	if(y_speed < y_speed_lower_limit)
	{
		y_speed_lower_limit = y_speed;
		
		// Calculate negative stepsize
		y_neg_stepsize = jabs(y_speed_lower_limit) / 100;
	}
	
	if(y_speed > y_speed_upper_limit)
	{
		y_speed_upper_limit = y_speed;
		
		// Calculate positive stepsize
		y_pos_stepsize = jabs(y_speed_upper_limit) / 100;
	}
	
	// Calculate relative speeds
	if(o < 0)
	{
		o = o / y_neg_stepsize;
	} else {
		o = o / y_pos_stepsize;
	}
	
	// Deadband
	if(jabs(o) < y_deadband)
	{
		o = 0;
	} else {
		if(o < 0)
		{
			o += y_deadband;
		} else {
			o -= y_deadband;
		}
	}
	
	// Saturation
	if(o < -99)
	{
		o = -99;
	}
	
	if(o > 99)
	{
		o = 99;
	}
	
	return o;
}

INT8S clean_x_speed ( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT16S o = x_speed;
	
	// Calculate stepsize
	if(x_neg_stepsize == 0)
	{
		x_neg_stepsize = jabs(x_speed_lower_limit) / 100;
	}
	
	if(x_pos_stepsize == 0)
	{
		x_pos_stepsize = x_speed_upper_limit / 100;
	}
	
	// Calculate stepsize if new limits
	if(x_speed < x_speed_lower_limit)
	{
		x_speed_lower_limit = x_speed;
		
		// Calculate negative stepsize
		x_neg_stepsize = jabs(x_speed_lower_limit) / 100;
	}
	
	if(x_speed > x_speed_upper_limit)
	{
		x_speed_upper_limit = x_speed;
		
		// Calculate positive stepsize
		x_pos_stepsize = jabs(x_speed_upper_limit) / 100;
	}
	
	// Calculate relative speeds
	if(o < 0)
	{
		o = o / x_neg_stepsize;
	} else {
		o = o / x_pos_stepsize;
	}
	
	// Deadband
	if(jabs(o) < x_deadband)
	{
		o = 0;
	} else {
		if(o < 0)
		{
			o += x_deadband;
		} else {
			o -= x_deadband;
		}
	}
	
	// Saturation
	if(o < -99)
	{
		o = -99;
	}
	
	if(o > 99)
	{
		o = 99;
	}
	
	return o;
}

void x_avg_calc ( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	static INT16S x_old_pos = 0;
	static INT8S old_rel_speed = 120;
	
	INT8U i;
	INT32U temp = 0;
	for( i = 0; i < BUFFER_SIZE; i++)
	{
		temp += x_values[i];
	}
	x_current = temp / BUFFER_SIZE;
	
	if(x_initialized)
	{
		if(x_old_pos == 0)
		{
			x_old_pos = x_current;
		}
		
		INT16S dpos = x_current - x_old_pos;
		INT16U a_dpos = jabs(dpos);
		
		
		if(dpos > 0 && a_dpos > 5)
		{
			// Negative delta-pos
			x_speed -= (a_dpos);
			x_old_pos = x_current;
		}

		if(dpos < 0 && a_dpos > 5)
		{	
			// Positive delta-pos
			x_speed += (a_dpos);
			x_old_pos = x_current;
		}
	}

	x_rel_speed = clean_x_speed();
	if(x_rel_speed != old_rel_speed)
	{
		write_2_char_signed_int_to_buffer (1, 0, x_rel_speed);
		old_rel_speed = x_rel_speed;
	}
}

void y_avg_calc ( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	static INT16S y_old_pos = 0;
	static INT8S old_rel_speed = 120;
	
	INT8U i;
	INT32U temp = 0;
	for( i = 0; i < BUFFER_SIZE; i++)
	{
		temp += y_values[i];
	}
	y_current = temp / BUFFER_SIZE;
	
	if(y_initialized)
	{
		if(y_old_pos == 0)
		{
			y_old_pos = y_current;
		}
		
		INT16S dpos = y_current - y_old_pos;
		INT16U a_dpos = jabs(dpos);
		
		
		if(dpos > 0 && a_dpos > 5)
		{
			// Negative delta-pos
			y_speed -= (a_dpos);
			y_old_pos = y_current;
		}

		if(dpos < 0 && a_dpos > 5)
		{	
			// Positive delta-pos
			y_speed += (a_dpos);
			y_old_pos = y_current;
		}
	}

	y_rel_speed = clean_y_speed();
	if(y_rel_speed != old_rel_speed)
	{
		write_2_char_signed_int_to_buffer (1, 1, y_rel_speed);
		old_rel_speed = y_rel_speed;
	}
}

void joystick_task( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	//static INT8U calc_counter = 0;
	
	ADC_PSSI_R |= ADC_PSSI_SS0;
	
	if(ADC_RIS_R && ADC_RIS_INR0)
	{
		//Bit cleared with IN0 bit, ADC_ISC_IN0, (0) i ADC_ISC_R. Write one to the bit for reset
		ADC_ISC_R |= ADC_ISC_IN0;
		//Data is read from ADC_SSFIFO0_R, with mask ADC_SSFIFO0_DATA_M (0x03FF)
		if(ADC_SSMUX0_R == 2)
		{
			if(x_pointer >= BUFFER_SIZE)
			{
				x_initialized = 1;
				x_pointer = 0;
			}
			x_values[x_pointer] = ADC_SSFIFO0_R & ADC_SSFIFO0_DATA_M;
			x_pointer++;
			x_avg_calc();
			
			ADC_SSMUX0_R = 3;
		}
		else
		{
			if(y_pointer >= BUFFER_SIZE)
			{
				y_initialized = 1;
				y_pointer = 0;
			}
			y_values[y_pointer] = ADC_SSFIFO0_R & ADC_SSFIFO3_DATA_M;
			y_pointer++;
			y_avg_calc();
				
			ADC_SSMUX0_R = 2;
		}
		//The converting starts by ,ADC_PSSI_SS0, bit 0 (for SS0) i ADC_PSSI_R
		ADC_PSSI_R |= ADC_PSSI_SS0;
	}
	
	calculate_v_space();
}

void init_joystick()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U dummy;
	INT8U aktive_ADC;
	
	SYSCTL_RCGC0_R |= 0x00010000; // ADC clock
	
	dummy = SYSCTL_RCGC2_R;

	//Disable sekvens for ADC0-ADC3 and stores if any of them are aktive
	aktive_ADC = 0b00001111 & ADC_ACTSS_R;
	ADC_ACTSS_R = 0;
	
	//set interrupt to controler controled
	ADC_EMUX_R = ADC_EMUX_EM1_PROCESSOR | ADC_EMUX_EM2_PROCESSOR;
	
	//For each sample in the sample sequence, configure the corresponding input source in the
	//ADCSSMUXn register.
	ADC_SSMUX0_R |= ADC_SSMUX0_MUX2_M & (0x01 << ADC_SSMUX0_MUX2_S);
	ADC_SSMUX0_R |= ADC_SSMUX0_MUX3_M & (0x01 << ADC_SSMUX0_MUX3_S);
	
	//Sets inti and end of conversion
	ADC_SSCTL0_R =  ADC_SSCTL0_IE0 | ADC_SSCTL0_END0;
	
	//enables ACD1 to secvenser 0
	aktive_ADC |= ADC_ACTSS_ASEN0;
	ADC_ACTSS_R = aktive_ADC;
	
	
	// Initiate first conversion:
	ADC_PSSI_R |= ADC_PSSI_SS0;
}

/****************************** End Of Module *******************************/
