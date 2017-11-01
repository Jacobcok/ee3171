#ifndef KEYPAD_H
#define KEYPAD_H
#include <stdint.h>
#include "sleep.h"

void SleepMS(uint32_t n);
void SleepUS(uint32_t n);

#define GPIO_PORTD_DATA_R  (*((volatile uint32_t *)0x400073FC))
#define GPIO_PORTD_DIR_R   (*((volatile uint32_t *)0x40007400))
#define GPIO_PORTD_DEN_R   (*((volatile uint32_t *)0x4000751C))
#define GPIO_PORTD_AMSEL_R (*((volatile uint32_t *)0x40007528))
#define GPIO_PORTA_DATA_R  (*((volatile uint32_t *)0x400043FC))
#define GPIO_PORTA_DIR_R   (*((volatile uint32_t *)0x40004400))
#define GPIO_PORTA_DEN_R   (*((volatile uint32_t *)0x4000451C))
#define GPIO_PORTA_AMSEL_R (*((volatile uint32_t *)0x40004528))
#define SYSCTL_RCGCGPIO_R  (*((volatile uint32_t *)0x400FE608))

#define ROW1 0x01
#define ROW2 0x02
#define ROW3 0x04
#define ROW4 0x08

const char keypadTable[17] = {0,'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};

void KeypadInit(void)
{
	SYSCTL_RCGCGPIO_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x08;
	
	GPIO_PORTA_DIR_R &= ~(0x3C);
	GPIO_PORTA_AMSEL_R &= ~(0x3C);
	GPIO_PORTA_DEN_R |= 0x3C;
	
	GPIO_PORTD_DIR_R &= ~(0x0F);
	GPIO_PORTD_AMSEL_R &= ~(0x0F);
	GPIO_PORTD_DEN_R |= 0x0F;
}

char KeypadScan(void)
{
		uint8_t row;
		uint8_t column;
		const uint8_t rowSelector[] = {ROW1, ROW2, ROW3, ROW4};
	
		// Check to see if any key is pressed
		GPIO_PORTD_DIR_R |= 0x0F;
		GPIO_PORTD_DATA_R |= 0x0F;
		column = GPIO_PORTA_DATA_R & 0x3C;
		GPIO_PORTD_DIR_R &= ~0x0F;
	
		// If column = 0, no key was pressed
		if (0 == column)
		{
			return 0;
		}
		
		// If a key is pressed, we have to figure out which key.
		// Activate 1 row at a time to determine what column is active.
		for (row = 0; row < 4; row++)
		{
				GPIO_PORTD_DIR_R &= ~0x0F;
				GPIO_PORTD_DIR_R = rowSelector[row];
				GPIO_PORTD_DATA_R |= 0x0F;
				SleepUS(2);
				column = GPIO_PORTA_DATA_R & 0x3C;
				if (0 != column) break;
		}
		
		GPIO_PORTD_DIR_R &= ~0x0F;
		if (4 == row)
		{
			return 0;
		}
		
		switch(column)
		{
		case 0x04:
			return row * 4 + 1;
		case 0x08:
			return row * 4 + 2;
		case 0x10:
			return row * 4 + 3;
		case 0x20:
			return row * 4 + 4;
		default:
			return 0;
	}
}

char KeypadTableLookup(uint8_t i)
{
	if (i < 16)
	{
		return keypadTable[i];
	}
	else
	{
		return 0;
	}
}

char KeypadRead()
{
	char lastChar;
	char thisChar;
	lastChar = KeypadScan();
	SleepMS(20);
	thisChar = KeypadScan();
	while (lastChar == thisChar)
	{
		lastChar = thisChar;
		SleepMS(20);
		thisChar = KeypadScan();
	}
	return thisChar;
}

#endif
