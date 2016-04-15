#include<stdint.h>
#include<stdio.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"inc/hw_gpio.h"

#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"

#include"inc/hw_ints.h"
#include"driverlib/interrupt.h"
#include"driverlib/uart.h"

#include"driverlib/debug.h"
#include"driverlib/sysctl.h"
#include"driverlib/adc.h"

#include"driverlib/rom.h"

volatile uint32_t ui32SetTempValueC = 25;

void printString(uint32_t cur) {
	if (cur == 1) {
		UARTCharPut(UART0_BASE, '\r');
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, 'u');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'n');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 'T');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'm');
		UARTCharPut(UART0_BASE, 'p');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, '=');
		UARTCharPut(UART0_BASE, ' ');
		return;
	} else if (cur == 2) {
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 'S');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 'T');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'm');
		UARTCharPut(UART0_BASE, 'p');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, '=');
		UARTCharPut(UART0_BASE, ' ');
	} else if (cur == 3) {
		UARTCharPut(UART0_BASE, '\r');
		UARTCharPut(UART0_BASE, 'E');
		UARTCharPut(UART0_BASE, 'n');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, 'h');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'm');
		UARTCharPut(UART0_BASE, 'p');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'a');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, 'u');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, ':');
		UARTCharPut(UART0_BASE, ' ');
	} else if (cur == 4) {
		UARTCharPut(UART0_BASE, '\r');
		UARTCharPut(UART0_BASE, 'S');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'm');
		UARTCharPut(UART0_BASE, 'p');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'a');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, 'u');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 'u');
		UARTCharPut(UART0_BASE, 'p');
		UARTCharPut(UART0_BASE, 'd');
		UARTCharPut(UART0_BASE, 'a');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'd');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, 'o');
		UARTCharPut(UART0_BASE, ' ');
	}
}

void setup(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
}

void led_pin_config(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

void configureADCSequencer(){
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
}

void configureUART() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

}

void printTemp(uint32_t temp) {
	if (temp < 10) {
		UARTCharPut(UART0_BASE, '0'+temp);
	} else {
		uint32_t firstDigit = temp/10;
		uint32_t secondDigit = temp%10;
		UARTCharPut(UART0_BASE, '0'+firstDigit);
		UARTCharPut(UART0_BASE, '0'+secondDigit);
	}
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, 248);
	UARTCharPut(UART0_BASE, 'C');
}

int main(void) {
	uint32_t ui32ADC0Value[4];
	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;

	setup();
	led_pin_config();
	configureADCSequencer();
	configureUART();

	IntMasterEnable();
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

	while(1)
	{
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false)){}

		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;

		if(ui32TempValueC < ui32SetTempValueC)
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 8); //green
		else
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 2); //red

		printString(1);
		printTemp(ui32TempValueC);
		UARTCharPut(UART0_BASE, ',');
		printString(2);
		printTemp(ui32SetTempValueC);
		UARTCharPut(UART0_BASE, '\n');
		SysCtlDelay(40000000/3);
	}
}

uint32_t readTemperature() {
	/*
	ui32SetTempValueC = 0;
	while(1){
		char input = UARTCharGet(UART0_BASE);
		if((input >= '0' && input <='9') || input == 13){
			UARTCharPut(UART0_BASE, input);
			if(input == 13) break;
			ui32SetTempValueC = 10 * ui32SetTempValueC + (input - '0');
		}
	}
	*/

	char firstChar = UARTCharGet(UART0_BASE);
	UARTCharPut(UART0_BASE, firstChar);
	char secondChar = UARTCharGet(UART0_BASE);
	UARTCharPut(UART0_BASE, secondChar);
	uint32_t first = firstChar - '0';
	uint32_t second =  secondChar -'0';

	return first*10+second;
}

void UARTIntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		char input = UARTCharGetNonBlocking(UART0_BASE);
		if (input  == 'S') {
			printString(3);
			ui32SetTempValueC = readTemperature();
			UARTCharPut(UART0_BASE, '\n');
			printString(4);
			printTemp(ui32SetTempValueC);
			UARTCharPut(UART0_BASE, '\n');
		};
		break;
	}
}
