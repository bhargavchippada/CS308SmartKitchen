#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/debug.h"
#include"driverlib/sysctl.h"
#include"driverlib/adc.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/uart.h"
#include"driverlib/rom.h"
/*
int main(void) {
	uint32_t ui32ADC0Value[4];
	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;

	setup();
	configureADCSequencer();
	configureUART();

	while(1)
	{
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);

		while(!ADCIntStatus(ADC0_BASE, 1, false)){}

		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;

		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;

		printString(1);
		printTemp(ui32TempValueC);
		UARTCharPut(UART0_BASE, '\n');
		SysCtlDelay(40000000/3);
	}
}
*/

