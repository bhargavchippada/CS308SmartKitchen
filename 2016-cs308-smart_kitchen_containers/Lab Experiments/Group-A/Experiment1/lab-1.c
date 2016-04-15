/*
 *
 * TeamId :			#Don't Know
 *
 * Author List:		Pranay Dhondi		(120050054)
 * 					Akhil Surya Vamshi	(120050067)
 *
 * Filename:		lab-1.c
 *
 * Theme:			None
 *
 * Functions:		setup(), ledPinConfig(),switchPinConfig(),
 					main()
 *
 * Global Variables: sw2Status,ui8LED
 *
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
*/
int32_t sw2Status=0; // This indicates how many times the Switch2 has pressed
uint8_t ui8LED = 2; // Led that has to glow on switch1 press

/*

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency and enable GPIO Peripherals  

* Example Call: setup();

*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*

* Function Name: ledPinConfig()

* Input: none

* Output: none

* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.

* Example Call: ledPinConfig();

*/

void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
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

* Function Name: main()

* Input: none

* Output: none

* Description: glows Led when switch1 is pressed and increases the sw2presscount on switch2 press

* Example Call: main();

*/

int main(void)
-{
	// Setting Things up
	setup();
	ledPinConfig();
	switchPinConfig();



	int32_t switch1,switch2; //switchx : Variable to read the 'Switchx' value
	bool ledOn = false,counting = false; // indicate the status of key press
	while (1) {
		switch1= GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 |GPIO_PIN_0 ); //Reading the switch1 port
		switch2 = switch1 & 1;
		switch1 = switch1 & 16;
		if (switch1 == 0) {// If it is pressed
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
			ledOn = true;
			SysCtlDelay(200000);
		} else if (switch1 == 16 && ledOn){ // If it is released just now
			ledOn = false;
			//Setting next led colour
			switch(ui8LED) {
			case 8:ui8LED = 4; break;
			case 4:ui8LED = 2; break;
			case 2:ui8LED = 8; break;
			}
			SysCtlDelay(200000);
		}else { // If it is not pressed
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
		}

		if (switch2 == 0 && !counting) {// If switch2 is released just now
			counting = true;
			sw2Status = sw2Status+1;// increasing the switch press count
			//SysCtlDelay(200000);
		} else if (switch2 == 0){

		}else{
			counting = false;
					//	SysCtlDelay(200000);
		}


	}


}
