#include <stdint.h>
#include "keypad.h"
#include "LCD.h"


#define GPIO_PORTB_DATA_R (*((volatile uint32_t *) 0x400053FC))
#define GPIO_PORTB_DIR_R (*((volatile uint32_t *) 0x40005400))
#define GPIO_PORTB_DEN_R (*((volatile uint32_t *) 0x4000551C))
	
char* name = "Jacob";
char* date = "11/08/2017";

int main()
{
	KeypadInit();
	LCDInit();
	//portConfig LED3-0 
	SYSCTL_RCGCGPIO_R |= 0x02;
	GPIO_PORTB_DIR_R |= 0x08;
	GPIO_PORTB_DEN_R |= 0x08;	
	
	while (1)
	{
		//read button press and display on LED3-0
		uint32_t buttonChar = KeypadRead();
		//clear LED
		GPIO_PORTB_DATA_R &= 0x00;
		//set LEDs according to button press
		GPIO_PORTB_DATA_R |= buttonChar;	
		
		LCDPrint(name, 1);
		LCDPrint(date, 2);
	}	
}
