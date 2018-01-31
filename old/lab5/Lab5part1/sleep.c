// Both the SleepMS and SleepUS functions are based on
// a 16 MHz clock.

void SleepMS(uint32_t n)
{
	volatile uint32_t i;
	volatile uint32_t j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < 3180; j++)
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
		for (j = 0; j < 3; j++)
		{
		}
	}
}
