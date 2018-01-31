#include <stdint.h>
#include "keypad.h"
#include "LCD.h"


#define GPIO_PORTB_DATA_R (*((volatile uint32_t *) 0x400053FC))
#define GPIO_PORTB_DIR_R (*((volatile uint32_t *) 0x40005400))
#define GPIO_PORTB_DEN_R (*((volatile uint32_t *) 0x4000551C))
	
int main()
{
	KeypadInit();
	//portConfig
	SYSCTL_RCGCGPIO_R |= 0x02;
	GPIO_PORTB_DIR_R |= 0x08;
	GPIO_PORTB_DEN_R |= 0x08;
	
	
	while (1)
	{
		uint32_t buttonChar = KeypadRead();
		GPIO_PORTB_DATA_R &= 0x00;
		GPIO_PORTB_DATA_R |= buttonChar;
	}	
}