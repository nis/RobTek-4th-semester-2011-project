/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: cpu.c
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


/*****************************    Defines    *******************************/

// PIN Defines
#define SSI_CLK	2
#define SSI_SS 	3
#define SSI_RX	4
#define SSI_TX 	5

// Pins in register
#define SSE_BIT 1

// The following defines is used in the Stellaris example project for SSI
// They might be necessary.

//#define SYSCTL_PERIPH_SSI0      0x10000010  // SSI 0

//*****************************************************************************
//
// The following are defines for the SSI register offsets.
//
//*****************************************************************************
// #define SSI_O_CR0               0x00000000  // SSI Control 0
// #define SSI_O_CR1               0x00000004  // SSI Control 1
// #define SSI_O_DR                0x00000008  // SSI Data
// #define SSI_O_SR                0x0000000C  // SSI Status
// #define SSI_O_CPSR              0x00000010  // SSI Clock Prescale
// #define SSI_O_IM                0x00000014  // SSI Interrupt Mask
// #define SSI_O_RIS               0x00000018  // SSI Raw Interrupt Status
// #define SSI_O_MIS               0x0000001C  // SSI Masked Interrupt Status
// #define SSI_O_ICR               0x00000020  // SSI Interrupt Clear
// #define SSI_O_DMACTL            0x00000024  // SSI DMA Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_CR1 register.
//
//*****************************************************************************
// #define SSI_CR1_EOT             0x00000010  // End of Transmission

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_DMACTL register.
//
//*****************************************************************************
// #define SSI_DMACTL_TXDMAE       0x00000002  // Transmit DMA Enable
// #define SSI_DMACTL_RXDMAE       0x00000001  // Receive DMA Enable

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void spi_receive_task( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT16U package;
	package = SSI0_DR_R;
	
	xQueueSend(spi_input_queue, &package, 0);
}

void spi_send_task( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT16U package;
	
	if(uxQueueMessagesWaiting(spi_output_queue) != 0) {
		xQueueReceive(spi_output_queue, &package, 0);
	} else {
		package = 0;
	}
	
	SSI0_DR_R = package;
}

void spi_enable( void )
/*****************************************************************************
*   Function : Enables SPI.
*****************************************************************************/
{
	SET_BIT(SSI0_CR1_R, SSE_BIT);
}

void spi_disable( void )
/*****************************************************************************
*   Function : Disables SPI.
*****************************************************************************/
{
	CLEAR_BIT(SSI0_CR1_R, SSE_BIT);
}

void init_spi()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U dummy;
	
	
	// Pin setup
	// Enable port A.
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
	dummy = SYSCTL_RCGC2_R;
	
	// HW control, Set pins in GPIO_PORTA_AFSEL_R
	SET_BIT(GPIO_PORTA_AFSEL_R, SSI_CLK);
	SET_BIT(GPIO_PORTA_AFSEL_R, SSI_SS);
	SET_BIT(GPIO_PORTA_AFSEL_R, SSI_RX);
	SET_BIT(GPIO_PORTA_AFSEL_R, SSI_TX);
	
	// Configure PAD: 	2mA drive: set pins in GPIO_PORTA_DR2R_R
	SET_BIT(GPIO_PORTA_DR2R_R, SSI_CLK);
	SET_BIT(GPIO_PORTA_DR2R_R, SSI_SS);
	SET_BIT(GPIO_PORTA_DR2R_R, SSI_RX);
	SET_BIT(GPIO_PORTA_DR2R_R, SSI_TX);
	//					Clear pins in GPIO_PORTA_ODR_R
	CLEAR_BIT(GPIO_PORTA_ODR_R, SSI_CLK);
	CLEAR_BIT(GPIO_PORTA_ODR_R, SSI_SS);
	CLEAR_BIT(GPIO_PORTA_ODR_R, SSI_RX);
	CLEAR_BIT(GPIO_PORTA_ODR_R, SSI_TX);
	//					Clear pins in GPIO_PORTA_PUR_R
	CLEAR_BIT(GPIO_PORTA_PUR_R, SSI_CLK);
	CLEAR_BIT(GPIO_PORTA_PUR_R, SSI_SS);
	CLEAR_BIT(GPIO_PORTA_PUR_R, SSI_RX);
	CLEAR_BIT(GPIO_PORTA_PUR_R, SSI_TX);
	//					Clear pins in GPIO_PORTA_PDR_R
	CLEAR_BIT(GPIO_PORTA_PDR_R, SSI_CLK);
	CLEAR_BIT(GPIO_PORTA_PDR_R, SSI_SS);
	CLEAR_BIT(GPIO_PORTA_PDR_R, SSI_RX);
	CLEAR_BIT(GPIO_PORTA_PDR_R, SSI_TX);
	//					Set pins in GPIO_PORTA_DEN_R
	SET_BIT(GPIO_PORTA_DEN_R, SSI_CLK);
	SET_BIT(GPIO_PORTA_DEN_R, SSI_SS);
	SET_BIT(GPIO_PORTA_DEN_R, SSI_RX);
	SET_BIT(GPIO_PORTA_DEN_R, SSI_TX);
	
	SET_BIT(GPIO_PORTA_DIR_R, SSI_CLK);
	SET_BIT(GPIO_PORTA_DIR_R, SSI_SS);
	SET_BIT(GPIO_PORTA_DIR_R, SSI_RX);
	SET_BIT(GPIO_PORTA_DIR_R, SSI_TX);
	
	
	// SSI setup
	
	// SYSCTL_RCGC1_R 	== RCGC1
	// SSI0_CR1_R 		== SSICR1
	// SSI0_CPSR_R 		== SSICPSR
	// SSI0_CR0_R		== SSICR0
	
	// Enable SSI Clock
	SYSCTL_RCGC1_R = SYSCTL_RCGC1_SSI0;
	
	// 1 Disable SSI
		// Clear SSI bit in SSICR1
	spi_disable();
	
	// 2 Select master or slave
		// For master operations, set the SSICR1 register to 0x0000.0000
	SSI0_CR1_R = 0x00000000;
	
	// 3 Configure clock
		// 50*10^6/(4*(1+12))
		// SysClock * 10^6 / (CPSDVSR * (1 + SCR))
		// (50 * (10^6)) / (4 * (1 + 9)) = 1 250 000 bit/sec
			
		// SSICPSR = CPSDVSR = 0x4
	SSI0_CPSR_R = 0x4;
		
	// 4 Write to SSICR0:
		// SCR: 	[15:8] SSI Serial Clock Rate
		// SPH: 	[7] SSI Serial Clock Phase
		// SPO:		[6]	SSI Serial Clock Polarity
		// FRF:		[5:4] FrameFormat, 0x0 for Freescale
		// DSS:		[3:0] Datasize
		// SSI0_CR0_R = 0b00000000 0 0 00 0000
	SSI0_CR0_R = 0b0000100100001111;
				 //0b00000100 1 1 00 1111
			
	// 5 Enable SSI
			// Set SSI bit in SSICR1
	spi_enable();


}

/****************************** End Of Module *******************************/
