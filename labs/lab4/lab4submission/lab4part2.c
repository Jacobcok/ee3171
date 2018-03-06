//*****************************************************************************
// Starter Code Foundation devlped by TI- This section gives credit to the starting point, which includeds the MM IO

// Copyright (c) 2013-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// This is part of revision 2.1.0.12573 of the Tiva Firmware Development Package.
//
//*****************************************************************************


//******************************************************************************
//*********Complete this section at the start of each project*******************
//*********Include a copy of this section in all program files you make*********
//
// Name: Jacob Cok
// Creation Date: 3/5/2018
// Lab Section: L02
// Lab this program is associated with: Lab 4
// Lab due date: March 
// 
// Hardware Inputs used:
//
// Hardware Outputs used:
//
// Additional files needed:
//
// Date of last modification: 3/5/2018
//
//********************************************************************************

//Include statements

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

// Put all #define statements here
// This is an example of converting an EQU statement into a #define
#define GPIO_PORTD_DATA_R  (*((volatile uint32_t *) 0x400073FC))
#define GPIO_PORTD_DIR_R   (*((volatile uint32_t *) 0x40007400))
#define GPIO_PORTD_DEN_R   (*((volatile uint32_t *) 0x4000751C))
#define GPIO_PORTD_AMSEL_R (*((volatile uint32_t *) 0x40007528))
#define GPIO_PORTB_DATA_R  (*((volatile uint32_t *) 0x400053FC))
#define GPIO_PORTB_DIR_R   (*((volatile uint32_t *) 0x40005400))
#define GPIO_PORTB_DEN_R   (*((volatile uint32_t *) 0x4000551C))
#define SYSCTL_RCGCGPIO_R  (*((volatile uint32_t *) 0x400FE608))


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
// Prototype Subroutines here if needed
void Board_Init(void);
void Time_Delay(void);

//*****************************************************************************
// Global Variable Declarations go here
volatile uint32_t i;
volatile uint32_t threshold = 642500;
// normal operation is 1, reverse direction is 2
volatile uint32_t direction; 
volatile uint32_t forward = 1;
volatile uint32_t reverse = 2;

//*****************************************************************************
//Main Program
int main(void)
{
	// Any initializations go here

	
	// Main code
	Board_Init();
	
	// port configuration
	// port b (LEDs)
	SYSCTL_RCGCGPIO_R |= 0x02;
	GPIO_PORTB_DIR_R  |= 0x0F;
	GPIO_PORTB_DEN_R  |= 0x0F;

	// port D (sw)
	SYSCTL_RCGCGPIO_R  |=   0x08;
	GPIO_PORTD_DIR_R   &= ~(0x0F);
	GPIO_PORTD_AMSEL_R &= ~(0x0F);
	GPIO_PORTD_DEN_R   |=   0x0F;		
	
	while(1) 	{
		Time_Delay();
		
		// SW4: PD2, SW5: PD3
		// if SW5 is pressed, sequence: LED0 -> 3
		while (direction == forward) {
			// turn on LED3
			GPIO_PORTB_DATA_R |= 0x08;
			Time_Delay();
			// turn off LEDs
			GPIO_PORTB_DATA_R &= ~(0x0F);
			
			// turn on LED2
			GPIO_PORTB_DATA_R |= 0x04;
			Time_Delay();
			// turn off LEDs
			GPIO_PORTB_DATA_R &= ~(0x0F);
			
			// turn on LED1
			GPIO_PORTB_DATA_R |= 0x02;
			Time_Delay();
			// turn off LEDs
			GPIO_PORTB_DATA_R &= ~(0x0F);
			
			// turn on LED0
			GPIO_PORTB_DATA_R |= 0x01;
			Time_Delay();
			// turn off LEDs
			GPIO_PORTB_DATA_R &= ~(0x0F);
		}			
	


	// if SW4 is pressed, sequence: LED3 -> 0
	while (direction == reverse) {
		// turn on LED0
		GPIO_PORTB_DATA_R |= 0x01;
		Time_Delay();
		// turn off LEDs
		GPIO_PORTB_DATA_R &= ~(0x0F);
		
		// turn on LED1
		GPIO_PORTB_DATA_R |= 0x02;
		Time_Delay();
		// turn off LEDs
		GPIO_PORTB_DATA_R &= ~(0x0F);
		
		// turn on LED2
		GPIO_PORTB_DATA_R |= 0x04;
		Time_Delay();
		// turn off LEDs
		GPIO_PORTB_DATA_R &= ~(0x0F);
		
		// turn on LED3
		GPIO_PORTB_DATA_R |= 0x08;
		Time_Delay();
		// turn off LEDs
		GPIO_PORTB_DATA_R &= ~(0x0F);
	}

		
		
	}
	

	return 0; //this is needed because the type of the function is "int"
}

void Board_Init(void)
{
	volatile int i = 0;

	return;
}

void Time_Delay(void) 
{
	for (i=0; i<threshold; i++) {
		if (GPIO_PORTD_DATA_R == 0x02){
			direction = forward;
		}
		if (GPIO_PORTD_DATA_R == 0x01){
			direction = reverse;
		}
	}
	return;
}
