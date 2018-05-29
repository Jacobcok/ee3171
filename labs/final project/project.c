//*****************************************************************************
// Author: Jacob Cok
// Date: 5/1/2018
// Final Project: Digital Thermometer
//
// Description: After a 5 sec timer, ADC read of thermometer sensor on AIN8.
//	1 sec later, turn on LED0. Display results of ADC read on LCD.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "inc/tm4c123gh6pm.h"


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
void LCD(void);

//*****************************************************************************

//*****************************************************************************
// Global Variable Declarations go here

uint32_t thermValue;
volatile uint32_t degreesF;
uint32_t tempTimer;
char stringDegrees1, stringDegrees0;
//*****************************************************************************

//*****************************************************************************
//
// Main Program
//
//*****************************************************************************
int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	
	// configure LEDs
	SYSCTL_RCGCGPIO_R |= 0x02;
	GPIO_PORTB_DIR_R  |= 0x0F;
	GPIO_PORTB_DEN_R  |= 0x0F;
	
	// configure timer
 	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
	TimerConfigure(WTIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC));
	TimerLoadSet(WTIMER0_BASE, TIMER_A, 80000000); // 5 sec timer (16MHz * 5)
	TimerLoadSet(WTIMER0_BASE, TIMER_B, 16000000); // 1 sec timer (16Mhz)
	TimerIntEnable(WTIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntEnable(WTIMER0_BASE, TIMER_TIMB_TIMEOUT);
	TimerEnable(WTIMER0_BASE, TIMER_A);
	IntEnable(INT_WTIMER0A);
	IntEnable(INT_WTIMER0B);
	
	// configure ADC for thermometer; AIN8 PE5
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);	
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);
	
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH8);
	ADCSequenceEnable(ADC0_BASE, 0);
	IntMasterEnable();
	
	// LCD init
	LCDInit();
	
	SysCtlDelay(3);
	
	while (1) {

		// used for debugging purposes
		tempTimer = TimerValueGet(WTIMER0_BASE, TIMER_A);

		
	}
	return 0;
}

void Thermometer(void){
	// trigger the sample sequence.
	ADCProcessorTrigger(ADC0_BASE, 0);
	
	// wait until sample sequence is completed
	while(!ADCIntStatus(ADC0_BASE, 0, false)) {}
	
	// read the value from the ADC
	ADCSequenceDataGet(ADC0_BASE, 0, &thermValue);
	
	// convert ADC to degrees
	degreesF = (thermValue/(4096/3.3)*100*9/5)+32;
	// 10s digit
	stringDegrees0 = degreesF/10+0x30;
	// 1s digit
	stringDegrees1 = degreesF%10+0x30;
	LCD();
	return;
}

void LCD(void) {
	LCDClear();
	LCDPrint("Degrees ", 1);
	LCDWriteData(stringDegrees0);
	LCDWriteData(stringDegrees1);
	
	
	return;
}

void TimerAISR(void) {
	TimerIntClear(WTIMER0_BASE, TIMER_TIMA_TIMEOUT);

	//start timer b
	TimerEnable(WTIMER0_BASE, TIMER_B);
	
	// turn off LED0
	GPIO_PORTB_DATA_R &= 0x00;
	
	// read the thermometer
	Thermometer();
}

void TimerBISR(void) {
	TimerIntClear(WTIMER0_BASE, TIMER_TIMB_TIMEOUT);
	
	// turn on LED0
	GPIO_PORTB_DATA_R |= 0x08;
	
	// stop timer b
	TimerDisable(WTIMER0_BASE, TIMER_B);
}
