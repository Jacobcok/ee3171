/* Lecture 9: polled i/o example */

#define PORTX_DIR_R 0xYYYYYYYY 
int main(void)
{
	// inits
	PORTX_DIR_R &= ~(0xFF); // ~11111111 -> 00000000, clears all bits
	PORTY_DIR_R &= ~(0X80); // 
	PORTZ_DIR_R |= 0xFF; 

	while (1)
	{
		// polling loop
		while (0x80 |= (0x80 & PORTY_FLAG_R)) {} // the & is used to look at the only interesting bit
		// could also do: while (0x80 |= (0x80 & PORTY_FLAG_R)) ; no need for the brackets if nothing is happening
		
		// clear the flag
		PORTY_FLAG_R |= 0x80;

		PORTZ_DATA_R = PORTX_DATA_R;

	}
}