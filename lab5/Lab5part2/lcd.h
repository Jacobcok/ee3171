#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include "sleep.h"

#define SYSCTL_RCGCSSI_R   (*((volatile uint32_t *)0x400FE61C))
#define SYSCTL_RCGCGPIO_R  (*((volatile uint32_t *)0x400FE608)) 
#define GPIO_PORTB_AMSEL_R (*((volatile uint32_t *)0x40005528))
#define GPIO_PORTB_AFSEL_R (*((volatile uint32_t *)0x40005420))
#define GPIO_PORTB_PCTL_R  (*((volatile uint32_t *)0x4000552C))
#define GPIO_PORTB_DEN_R   (*((volatile uint32_t *)0x4000551C))

#define GPIO_PORTC_AMSEL_R (*((volatile uint32_t *)0x40006528))
#define GPIO_PORTC_AFSEL_R (*((volatile uint32_t *)0x40006420))
#define GPIO_PORTC_PCTL_R  (*((volatile uint32_t *)0x4000652C))
#define GPIO_PORTC_DATA_R  (*((volatile uint32_t *)0x400063FC))
#define GPIO_PORTC_DIR_R   (*((volatile uint32_t *)0x40006400))
#define GPIO_PORTC_DEN_R   (*((volatile uint32_t *)0x4000651C))

#define SSI2_CR0_R         (*((volatile uint32_t *)0x4000A000))
#define SSI2_CR1_R         (*((volatile uint32_t *)0x4000A004))
#define SSI2_DR_R          (*((volatile uint32_t *)0x4000A008))
#define SSI2_SR_R          (*((volatile uint32_t *)0x4000A00C))
#define SSI2_CPSR_R        (*((volatile uint32_t *)0x4000A010))
#define SSI2_IM_R          (*((volatile uint32_t *)0x4000A014))
#define SSI2_RIS_R         (*((volatile uint32_t *)0x4000A018))
#define SSI2_MIS_R         (*((volatile uint32_t *)0x4000A01C))
#define SSI2_ICR_R         (*((volatile uint32_t *)0x4000A020))
#define SSI2_DMACTL_R      (*((volatile uint32_t *)0x4000A024))
#define SSI2_CC_R          (*((volatile uint32_t *)0x4000AFC8))

#define RS 1 // Mask for LCD reg select
#define EN 2 // Mask for LCD E bit
#define BL 4 // Mask for LCD backlight
#define LINE1 1 
#define LINE2 2

void LCDInit(void);
void SSI2Write(unsigned char data);
void LCDWriteNibble(char data, uint8_t control); 
void LCDWriteCommand(unsigned char command);
void LCDWriteData(char data);

void LCDInit(void)
{
	// Turn on the clocks to the SSI module and GPIO modules
	SYSCTL_RCGCSSI_R |= 0x04;
	SYSCTL_RCGCGPIO_R |= 0x02;
	SYSCTL_RCGCGPIO_R |= 0x04;
	
	// Port B initializations
	// Turn off analog mode,set pins 7 & 4 to SSI2 mode, digital inputs
	GPIO_PORTB_AMSEL_R &= ~0x90;
	GPIO_PORTB_AFSEL_R |= 0x90;
	GPIO_PORTB_PCTL_R &= ~0xF00F0000;
	GPIO_PORTB_PCTL_R |= 0x20020000;
	GPIO_PORTB_DEN_R |= 0x90;
	
	// Port C initializations
	// Turn off analog mode, configure pin 6 for SSI slave select
	GPIO_PORTC_AMSEL_R &= ~0x40;
	GPIO_PORTC_DATA_R |= 0x40;
	GPIO_PORTC_DIR_R |= 0x40;
	GPIO_PORTC_DEN_R  |= 0x40;
	
	// SSI2 Initializations
	SSI2_CR1_R = 0x00;
	SSI2_CC_R = 0x00;
	SSI2_CPSR_R = 16;
	SSI2_CR0_R = 0x0007;
	SSI2_CR1_R = 0x02;
	
	// This funny sequence resets the LCD controller
	SleepMS(20);
	LCDWriteNibble(0x30, 0);
	SleepMS(5);
	LCDWriteNibble(0x30, 0);
	SleepMS(1);
	LCDWriteNibble(0x30, 0);
	SleepMS(1);
	
	// Set the LCD controller for 4-bit data mode
	LCDWriteNibble(0x20, 0);
	SleepMS(1);
	
	// Configure the LCD:
	// Set up for 2 lines of display and 5x7 font
	LCDWriteCommand(0x28);
	
	// Move the cursor to the right
	LCDWriteCommand(0x06);
	
	// Clear the screen and reset cursor to home
	LCDWriteCommand(0x01);
	
	// Turn on the display, start cursor blinking
	LCDWriteCommand(0x0F);
	
	return;
}

void SSI2Write(unsigned char data)
{
	// Assert the SSI Chip Select (active low)
	GPIO_PORTC_DATA_R &= ~0x40;
	
	// Send the actual data
	SSI2_DR_R = data;
	
	// Wait for it to be done transmitting
	while (0x10 == (SSI2_SR_R & 0x10)) ;
	
	// Clear the chip select
	GPIO_PORTC_DATA_R |= 0x40;
	
	return;
}

void LCDWriteNibble(char data, uint8_t control)
{
	// Prep the data by clearing upper nibble (control)
	// and lower nibble (data)
	data &= 0xF0;
	control &= 0x0F;
	
	// Convoluted process to write data to LCD
	SSI2Write(data | control | BL);
	SSI2Write(data | control | EN | BL);
	SleepMS(0);
	SSI2Write(data | BL);
	SSI2Write(BL);
	
	return;
}

void LCDWriteCommand(unsigned char command)
{
	// Write the 8-bit command as 2 4-bit nibbles (sequentially)
	LCDWriteNibble((command & 0xF0), 0);
	LCDWriteNibble((command << 4), 0);
	
	// Wait for the LCD to respond
	if (command < 4)
	{
		SleepMS(2);
	}
	else
	{
		SleepMS(1);
	}
	
	return;
}

void LCDWriteData(char data)
{
	volatile char myChar = data & 0xF0;
	LCDWriteNibble((data & 0xF0), RS);
	LCDWriteNibble((data << 4), RS);
	
	SleepMS(40);
}

void LCDPrint(char* string, uint8_t line)
{
	char* currChar = string;
	if (LINE1 == line)
	{
		LCDWriteCommand(0x80);
	}
	else if (LINE2 == line)
	{
		LCDWriteCommand(0xC0);
	}
	else
	{
		return;
	}
	while ('\0' != *currChar)
	{
		LCDWriteData(*currChar);
		currChar++;
	}
}

void LCDClear(void)
{
		LCDWriteCommand(0x01);
}

#endif
