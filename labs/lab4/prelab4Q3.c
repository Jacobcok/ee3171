// counter must be volatile to prevent the compiler
// from removing a  useless loop.
volatile uint32_t i;
uint32_t threshold = 10000;
// the semicolon means the body of the loop is empty
for (i=0; i<threshold; i++) ;

// SW4: PD2, SW5: PD3
// if SW5 is pressed, sequence: LED3 -> 0
if (GPIO_PORTD_DATA_R == 0x08) {
	// turn on LED3
	GPIO_PORTB_DATA_R |= 0x08;
	for (i=0; i<threshold; i++) ;
	// turn off LED3 and turn on LED2
	GPIO_PORTB_DATA_R |= 0x04;
	for (i=0; i<threshold; i++) ;
	// turn off LED2 and turn on LED1
	GPIO_PORTB_DATA_R |= 0x02;
	for (i=0; i<threshold; i++) ;
	// turn off LED1 and turn on LED0
	GPIO_PORTB_DATA_R |= 0x01;
	for (i=0; i<threshold; i++) ;	
}
// if SW4 is pressed, sequence: LED0 -> 3
if (GPIO_PORTD_DATA_R == 0x04) {
	// turn on LED3
	GPIO_PORTB_DATA_R |= 0x01;
	for (i=0; i<threshold; i++) ;
	// turn off LED3 and turn on LED2
	GPIO_PORTB_DATA_R |= 0x02;
	for (i=0; i<threshold; i++) ;
	// turn off LED2 and turn on LED1
	GPIO_PORTB_DATA_R |= 0x04;
	for (i=0; i<threshold; i++) ;
	// turn off LED1 and turn on LED0
	GPIO_PORTB_DATA_R |= 0x08;
	for (i=0; i<threshold; i++) ;	
}
