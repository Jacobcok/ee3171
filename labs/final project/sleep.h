// Both the SleepMS and SleepUS functions are based on
// an 80 MHz clock.
#ifndef SLEEP_H
#define SLEEP_H

#include<stdint.h>

void SleepMS(uint32_t n)
{
	volatile uint32_t i;
	volatile uint32_t j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < 15900; j++)
		{
		}
	}
}

void SleepUS(uint32_t n)
{
	volatile uint32_t i;
	volatile uint32_t j;
	
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < 15; j++)
		{
		}
	}
}
#endif
