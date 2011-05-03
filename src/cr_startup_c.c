//*****************************************************************************
//   +--+       
//   | ++----+   
//   +-++    |  
//     |     |  
//   +-+--+  |   
//   | +--+--+  
//   +----+    Copyright (c) 2009-10 Code Red Technologies Ltd.
//
// Microcontroller Startup code for use with Red Suite
//
// Software License Agreement
//
// The software is owned by Code Red Technologies and/or its suppliers, and is
// protected under applicable copyright laws.  All rights are reserved.  Any
// use in violation of the foregoing restrictions may subject the user to criminal
// sanctions under applicable laws, as well as to civil liability for the breach
// of the terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD.
//
//*****************************************************************************
#if defined (__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#else
//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern "C" {
extern void __libc_init_array(void);
}
#endif
#endif

#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

// Code Red - if CMSIS is being used, then SystemInit() routine
// will be called by startup code rather than in application's main()
#if defined (__USE_CMSIS)
#include "system_LM3S.h"
#endif

//*****************************************************************************
#if defined (__cplusplus)
extern "C" {
#endif

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void ResetISR(void);
void NmiSR(void);
void FaultISR(void);
void IntDefaultHandler(void);

//*****************************************************************************
//
// External declarations for the interrupt handlers used by the application.
//
//*****************************************************************************
extern void xPortSysTickHandler(void);
extern void xPortPendSVHandler(void);
extern void vPortSVCHandler( void );

//*****************************************************************************
//
// The entry point for the application.
// __main() is the entry point for Redlib based applications
// main() is the entry point for Newlib based applications
//
//*****************************************************************************
#if defined (__REDLIB__)
extern void __main(void);
#endif
extern int main(void);
//*****************************************************************************
//
// External declaration for the pointer to the stack top from the Linker Script
//
//*****************************************************************************
extern void _vStackTop(void);

//*****************************************************************************
#if defined (__cplusplus)
} // extern "C"
#endif
//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
	//
		&_vStackTop, // The initial stack pointer
		ResetISR, // The reset handler
		NmiSR, // The NMI handler
		FaultISR, // The hard fault handler
		IntDefaultHandler, // The MPU fault handler
		IntDefaultHandler, // The bus fault handler
		IntDefaultHandler, // The usage fault handler
		0, // Reserved
		0, // Reserved
		0, // Reserved
		0, // Reserved
		vPortSVCHandler, // SVCall handler
		IntDefaultHandler, // Debug monitor handler
		0, // Reserved
		xPortPendSVHandler, // The PendSV handler
		xPortSysTickHandler, // The SysTick handler
		IntDefaultHandler, // GPIO Port A
		IntDefaultHandler, // GPIO Port B
		IntDefaultHandler, // GPIO Port C
		IntDefaultHandler, // GPIO Port D
		IntDefaultHandler, // GPIO Port E
		IntDefaultHandler, // UART0 Rx and Tx
		IntDefaultHandler, // UART1 Rx and Tx
		IntDefaultHandler, // SSI Rx and Tx
		IntDefaultHandler, // I2C Master and Slave
		IntDefaultHandler, // PWM Fault
		IntDefaultHandler, // PWM Generator 0
		IntDefaultHandler, // PWM Generator 1
		IntDefaultHandler, // PWM Generator 2
		IntDefaultHandler, // Quadrature Encoder
		IntDefaultHandler, // ADC Sequence 0
		IntDefaultHandler, // ADC Sequence 1
		IntDefaultHandler, // ADC Sequence 2
		IntDefaultHandler, // ADC Sequence 3
		IntDefaultHandler, // Watchdog timer
		IntDefaultHandler, // Timer 0 subtimer A
		IntDefaultHandler, // Timer 0 subtimer B
		IntDefaultHandler, // Timer 1 subtimer A
		IntDefaultHandler, // Timer 1 subtimer B
		IntDefaultHandler, // Timer 2 subtimer A
		IntDefaultHandler, // Timer 2 subtimer B
		IntDefaultHandler, // Analog Comparator 0
		IntDefaultHandler, // Analog Comparator 1
		IntDefaultHandler, // Analog Comparator 2
		IntDefaultHandler, // System Control (PLL, OSC, BO)
		IntDefaultHandler, // FLASH Control
		IntDefaultHandler, // GPIO Port F
		IntDefaultHandler, // GPIO Port G
		IntDefaultHandler, // GPIO Port H
		IntDefaultHandler, // UART2 Rx and Tx
		IntDefaultHandler, // SSI1 Rx and Tx
		IntDefaultHandler, // Timer 3 subtimer A
		IntDefaultHandler, // Timer 3 subtimer B
		IntDefaultHandler, // I2C1 Master and Slave
		IntDefaultHandler, // Quadrature Encoder 1
		IntDefaultHandler, // CAN0
		IntDefaultHandler, // CAN1
		0, // Reserved
		IntDefaultHandler, // Ethernet
		IntDefaultHandler // Hibernate
	};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;

//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++
// library.
//
//*****************************************************************************
void ResetISR(void) {
	unsigned long *pulSrc, *pulDest;

	//
	// Copy the data segment initializers from flash to SRAM.
	//
	pulSrc = &_etext;
	for (pulDest = &_data; pulDest < &_edata;) {
		*pulDest++ = *pulSrc++;
	}

	//
	// Zero fill the bss segment.  This is done with inline assembly since this
	// will clear the value of pulDest if it is not kept in a register.
	//
	__asm("    ldr     r0, =_bss\n"
			"    ldr     r1, =_ebss\n"
			"    mov     r2, #0\n"
			"    .thumb_func\n"
			"zero_loop:\n"
			"        cmp     r0, r1\n"
			"        it      lt\n"
			"        strlt   r2, [r0], #4\n"
			"        blt     zero_loop");

#ifdef __USE_CMSIS
	SystemInit();
#endif

#if defined (__cplusplus)
	//
	// Call C++ library initialisation
	//
	__libc_init_array();
#endif

#if defined (__REDLIB__)
	// Call the Redlib library, which in turn calls main()
	__main();
#else
	main();
#endif

	//
	// main() shouldn't return, but if it does, we'll just enter an infinite loop
	//
	while (1) {
		;
	}
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
void NmiSR(void) {
	//
	// Enter an infinite loop.
	//
	while (1) {
	}
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
void FaultISR(void) {
	//
	// Enter an infinite loop.
	//
	while (1) {
	}
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
void IntDefaultHandler(void) {
	//
	// Go into an infinite loop.
	//
	while (1) {
	}
}
