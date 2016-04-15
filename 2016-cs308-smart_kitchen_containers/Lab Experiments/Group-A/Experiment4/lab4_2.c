/*
 *
 * TeamId :			#Don't Know
 *
 * Author List:		Pranay Dhondi		(120050054)
 * 					Akhil Surya Vamshi	(120050067)
 *
 * Filename:		lab4_2.c
 *
 * Theme:			None
 *
 * Functions:		printString(uint32_t),printTemp(uint32_t),
 					readTemperature(),UARTIntHandler(),
 					switchPinConfig(),configureLED(),main()

 *
 * Global Variables: ui32ADC0Value,ui32TempAvg,
 					ui32TempValueC,ui32TempValueF,setTemp
 *
*/
// Including headers
#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/debug.h"
#include"driverlib/sysctl.h"
#include"driverlib/adc.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"inc/hw_ints.h"
#include"driverlib/interrupt.h"
#include"driverlib/uart.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

// Global Variables
uint32_t ui32ADC0Value[4]; //ui32ADC0Value :- To read Values from ADC
volatile uint32_t ui32TempAvg; // ui32TempAvg : Average of all reading read by ADC 
volatile uint32_t ui32TempValueC; // ui32TempValueC : Temperature value in Celsius
volatile uint32_t ui32TempValueF;  // ui32TempValueF : Temperature value in Fahrenheit 
volatile uint32_t setTemp =25; //setTemp: Temperate to be set 


//Functions

/*

* Function Name: printString(uint32_t)

* Input: mode of temperature Sensor, domain:{1,2,3,4}

* Output: Prints the command corresponding to mode

* Description: Uses inbuilt UARTCharPut to put each and every bit to transmitter

* Example Call: printString(1);

*/
void printString(uint32_t cur) {
	if (cur == 1) {
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


/*

* Function Name: printTemp(uint32_t)

* Input: temperature (<100)(>=0)

* Output: Outputs entered value to Transmitter

* Logic: Splits input(integer) into char and then transmits to the transmitter using UARTCharPut

* Example Call: printTemp(20);

*/

void printTemp(uint32_t temp) {
	if (temp >= 10) {
		uint32_t lastDigit = temp%10;
		printTemp(temp/10);
		UARTCharPut(UART0_BASE, '0'+ lastDigit);
		return;
	}
	UARTCharPut(UART0_BASE, '0'+temp);

}

/*

* Function Name: readTemperature()

* Input: none

* Output: returns unsigned 32bit integer: read from Reciever

* Description: Uses inbuilt UARTCharGet to character from reciever and then converts bits into 32bit integer

* Example Call: readTemperature();

*/


uint32_t readTemperature() {
	bool positive = true;
	uint32_t temp = 0;
	char cur;
	while(true) {
		cur = UARTCharGet(UART0_BASE);
		UARTCharPut(UART0_BASE, cur);
		if (cur == '-') {
			positive = false;
		} if (cur == '\r') {
			UARTCharPut(UART0_BASE, '\n');
			if (!positive) {
				return -temp;
			} else {
				return temp;
			}
		} else {
			int num = cur -'0';
			temp = temp*10+num;
		}
	}


//	char firstChar = UARTCharGet(UART0_BASE);
//	UARTCharPut(UART0_BASE, firstChar);
//	char secondChar = UARTCharGet(UART0_BASE);
//	UARTCharPut(UART0_BASE, secondChar);
//	uint32_t first = firstChar - '0';
//	uint32_t second =  secondChar -'0';
//
//	return first*10+second;
}

/*

* Function Name: UARTIntHandler()

* Input: none

* Output: none


* Description: Interupt handler for UART, checks whether the input is 'S', 
				if it is then reads the temperature from user.


* Example Call: UARTIntHandler();

*/

void UARTIntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		char input = UARTCharGetNonBlocking(UART0_BASE);
		if (input  == 'S') {
			printString(3); //printing/senging user string "Enter Temperature:";
			setTemp = readTemperature();
			printString(4); //printing/senging user string "Set Tempearture is update to";
			printTemp(setTemp);
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, 186);
			UARTCharPut(UART0_BASE, 'C');
			UARTCharPut(UART0_BASE, '\r');
			UARTCharPut(UART0_BASE, '\n');
		};
		return;
	}
}

/*

 * Function Name: switchPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 0 and Pin 4 as input. Note that Pin 0 is locked.

 * Example Call: switchPinConfig();

 */

void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);

}


/*

* Function Name: configureLED()

* Input: none

* Output: none

* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output,PORTF Pin 0, Pin 4 as input.

* Example Call: configureLED();

*/

void configureLED() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	switchPinConfig();
}

/*
 * Function Name:- main
 *
 * Input:			No arguments needed
 *
 * Ouptut:-			none
 *
 * Logic:-			Sets up all the harware used, enables the interupts, and the initial states of the program
 *
 * Example Call:	called automatically by the OS
 *
 */


int main(void) {

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // Set crystal frequency 

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); // Set/enable ADC0

	//Setting properties of ADC0
	ADCHardwareOversampleConfigure(ADC0_BASE, 64); 
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);

	ADCSequenceEnable(ADC0_BASE, 1);

	//Configuring UART
	configureUART();
	//Configuring GPIO pins of LEDS
	configureLED();
	//Starting the InterruptMaster and UART Interrupt
	IntMasterEnable();
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	uint8_t ui8LED = 2;


	while(1)
	{
		ADCIntClear(ADC0_BASE, 1); 
		ADCProcessorTrigger(ADC0_BASE, 1); // Initializing ADC processor trigger

		while(!ADCIntStatus(ADC0_BASE, 1, false)) //Waiting till interrupt
		{
		}
		// Reading values from ADC and taking average and convering to *C and *F
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;

		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

		//Printing the obtained value
		uint32_t firstDigit = ui32TempValueC/10;
		uint32_t secondDigit = ui32TempValueC%10;
		printString(1);
		printTemp(ui32TempValueC);
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 186);
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE,',');
		UARTCharPut(UART0_BASE,' ');
		printString(2);
		printTemp(setTemp);
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 186);
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, '\r');
		UARTCharPut(UART0_BASE, '\n');
		if(ui32TempValueC < setTemp) {
			ui8LED = 8;
		}
		else {
			ui8LED = 2;
			//TODO red LED should glow
		}
		//Changing color of LED 
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);

		SysCtlDelay(10000000);

//		if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
	}
}
