#define GPIO_PORTD_DATA_R (*((volatile uint32_t *) 0x400073FC
#define GPIO_PORTD_DIR_R (*((volatile uint32_t *) 0x40007400
#define GPIO_PORTD_DEN_R (*((volatile uint32_t *) 0x4000751C
#define GPIO_PORTD_AMSEL_R (*((volatile uint32_t *) 0x40007528
#define GPIO_PORTB_DATA_R (*((volatile uint32_t *) 0x400053FC
#define GPIO_PORTB_DIR_R (*((volatile uint32_t *) 0x40005400
#define GPIO_PORTB_DEN_R (*((volatile uint32_t *) 0x4000551C
#define SYSCTL_RCGCGPIO_R (*((volatile uint32_t *) 0x400FE608

uint32_t sw4 = 0;
uint32_t sw5 = 0;

int main()
{
	while (1)
	{
		//check if SW4 is pressed
		if (GPIO_PORTD_DATA_R == 0x02)
		{
			sw4==1;
		}
		else
		{
			sw4==0;
		}

		//check if SW5 is pressed
		if (GPIO_PORTD_DATA_R == 0x01)
		{
			//turn off all leds
			sw4==0;
		}


		if (sw4==1)
		{	
		//turn only LED3 on
		GPIO_PORTB_DATA_R = 0x08;
		}


		if (sw4==1)
		{
		//delay
		volatile uint32_t i;
		uint32_t threshold = 10000;
		for (i=0; i< threshold; i++);
		}

		//check if SW5 is pressed
		if (GPIO_PORTD_DATA_R == 0x01)
		{
			//turn off all leds
			sw4==0;
		}


		if (sw4==1)
		{
		//turn only LED2 on
		GPIO_PORTB_DATA_R = 0x04;
		}

		if (sw4==1)
		{
		//delay
		volatile uint32_t i;
		uint32_t threshold = 10000;
		for (i=0; i< threshold; i++);
		}

		//check if SW5 is pressed
		if (GPIO_PORTD_DATA_R == 0x01)
		{
			//turn off all leds
			sw4==0;
		}

		if (sw4==1)
		{
		//turn only LED1 on
		GPIO_PORTB_DATA_R = 0x02;
		}

		if (sw4==1)
		{
		//delay
		volatile uint32_t i;
		uint32_t threshold = 10000;
		for (i=0; i< threshold; i++);
		}

		//check if SW5 is pressed
		if (GPIO_PORTD_DATA_R == 0x01)
		{
			//turn off all leds
			sw4==0;
		}
		
		if (sw4==1)
		{
		//turn only LED0 on
		GPIO_PORTB_DATA_R = 0x01;
		}

		if (sw4==1)
		{
		//delay
		volatile uint32_t i;
		uint32_t threshold = 10000;
		for (i=0; i< threshold; i++);
		}

	}
}

//portConfig
// Port B initializations first
SYSCTL_RCGCGPIO_R |= 0x02;
GPIO_PORTB_DIR_R |= 0x08;
GPIO_PORTB_DEN_R |= 0x08;

// Port D init
SYSCTL_RCGCGPIO_R |= 0x08;
GPIO_PORTD_DIR_R |= 0x0F;
GPIO_PORTD_AMSEL_R |= 0x0F;
GPIO_PORTD_DEN_R |= 0x0F;