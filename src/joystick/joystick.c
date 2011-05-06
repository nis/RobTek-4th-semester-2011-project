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

#define BUFFER_SIZE 10

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT16U x_current = 0;
INT16U y_current = 0;

INT16U x_values[BUFFER_SIZE];
INT8U x_pointer = 0;

INT16U y_values[BUFFER_SIZE];
INT8U y_pointer = 0;

/*****************************   Functions   *******************************/

void x_avg_calc ( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U i;
	INT32U temp = 0;
	
	for( i = 0; i < BUFFER_SIZE; i++)
	{
		temp += x_values[i];
	}
	
	x_current = temp / BUFFER_SIZE;
}

void y_avg_calc ( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U i;
	INT32U temp = 0;
	
	for( i = 0; i < BUFFER_SIZE; i++)
	{
		temp += y_values[i];
	}
	
	y_current = temp / BUFFER_SIZE;
}

void joystick_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
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
				x_pointer = 0;
			}
			x_values[x_pointer] = ADC_SSFIFO0_R & ADC_SSFIFO0_DATA_M;
			x_pointer++;
			x_avg_calc();
			write_4_char_int_to_buffer (3, 0, x_current );
			
			ADC_SSMUX0_R = 3;
		}
		else
		{
			if(y_pointer >= BUFFER_SIZE)
			{
				y_pointer = 0;
			}
			y_values[y_pointer] = ADC_SSFIFO0_R & ADC_SSFIFO3_DATA_M;
			y_pointer++;
			y_avg_calc();
			write_4_char_int_to_buffer (12, 0, y_current );
				
			ADC_SSMUX0_R = 2;
		}
		//The converting starts by ,ADC_PSSI_SS0, bit 0 (for SS0) i ADC_PSSI_R
		ADC_PSSI_R |= ADC_PSSI_SS0;
	}
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
