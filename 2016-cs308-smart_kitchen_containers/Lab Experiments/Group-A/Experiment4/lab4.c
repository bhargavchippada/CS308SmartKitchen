/*
 *
 * TeamId :			#Don't Know
 *
 * Author List:		Pranay Dhondi		(120050054)
 * 					Akhil Surya Vamshi	(120050067)
 *
 * Filename:		lab4.c
 *
 * Theme:			None
 *
 * Functions:		configureUART(),putStart()
 *
 * Global Variables: ui32ADC0Value,ui32TempAvg,
 					ui32TempValueC,ui32TempValueF
 *
*/

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
#include"inc/hw_ints.h"
#include"driverlib/interrupt.h"

uint32_t ui32ADC0Value[4]; //ui32ADC0Value :- To read Values from ADC
volatile uint32_t ui32TempAvg; // ui32TempAvg : Average of all reading read by ADC 
volatile uint32_t ui32TempValueC; // ui32TempValueC : Temperature value in Celsius
volatile uint32_t ui32TempValueF;  // ui32TempValueF : Temperature value in Fahrenheit 


/*

* Function Name: configureUART()

* Input: none

* Output: none

* Description: Enables the port corresponding to UART, Sets Reciever and transmitter ports to
			 	portA pin0 and portA pint0 respectively. Setting baudrate,parity etc for UART

* Example Call: configureUART();

*/

void configureUART() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

}
/*

* Function Name: putStart()

* Input: none

* Output: Puts "Current Temperature " in Transmitter

* Description: Uses inbuilt UARTCharPut to put each and every bit to transmitter

* Example Call: putStart();

*/

void putStart() {
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
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'r');
	UARTCharPut(UART0_BASE, 'a');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, 'u');
	UARTCharPut(UART0_BASE, 'r');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, ' ');

}

//int main(void) {
//
//	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
//
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
//	ADCHardwareOversampleConfigure(ADC0_BASE, 64);
//	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
//
//	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
//	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
//	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
//	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
//
//	ADCSequenceEnable(ADC0_BASE, 1);
//
//	configureUART();
//
//
//
//	while(1)
//	{
//		ADCIntClear(ADC0_BASE, 1);
//		ADCProcessorTrigger(ADC0_BASE, 1);
//
//		while(!ADCIntStatus(ADC0_BASE, 1, false))
//		{
//		}
//
//		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
//
//		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
//
//		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
//		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
//
//		uint32_t firstDigit = ui32TempValueC/10;
//		uint32_t secondDigit = ui32TempValueC%10;
//		putStart();
//		UARTCharPut(UART0_BASE, '0'+firstDigit);
//		UARTCharPut(UART0_BASE, '0'+secondDigit);
//		UARTCharPut(UART0_BASE, 167);
//		UARTCharPut(UART0_BASE, 'C');
//		UARTCharPut(UART0_BASE, '\r');
//		UARTCharPut(UART0_BASE, '\n');
//		SysCtlDelay(1000000);
//
//		if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
//	}
//}
