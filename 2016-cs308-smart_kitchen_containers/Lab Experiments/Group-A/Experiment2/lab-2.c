/*
 *
 * TeamId :			#Don't Know
 *
 * Author List:		Pranay Dhondi		(120050054)
 * 					Akhil Surya Vamshi	(120050067)
 *
 * Filename:		lab-2.c
 *
 * Theme:			None
 *
 * Functions:		setup(), ledPinConfig(),switchPinConfig(),
 *					detectKeyPress(),detectKey2Press(),
 *					Timer0IntHandler(),main()
 *
 * Global Variables: ui32Period,flag,flag1,ui8LED,sw2Status,
 * 				     ledOn,counting
 *
*/

//Including Headers
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"


// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))



/*
 ------ Global Variable Declaration
*/

uint32_t ui32Period;// period of timer
unsigned char flag=0,flag1=0; //flag :- no.of consecutive switch1 pressed detected in Timeout interrupts
uint8_t ui8LED = 2; // next led color when switch1 is pressed
int32_t sw2Status=0; // how many times switch2 is pressed
bool ledOn = false,counting = false; //ledOn- indicates whether led is on/switch1 is pressed, counting- whether the switch2 is being pressed




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
 * Function Name:- timerConfig
 *
 * Input:			No arguments needed
 *
 * Ouptut:-			setting the timer
 *
 * Logic:-			enabling the imer interface and setting the type of timer
 *
 * Example Call:	timerConfig()
 *
 */


void timerConfig(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
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
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4 | GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

/*
 * Function Name:- detectKeyPress
 *
 * Input:			No arguments needed
 *
 * Ouptut:-			returns 1 if the switch1 is kept pressed for 2 timer interupts else returns 0
 *
 * Logic:-			flag is a variable which is used to know how many interupts it has been found
 * 					pressed and its count is used to return ouput
 *
 * Example Call:	detectKeyPress()
 *
 */


unsigned char detectKeyPress()
{
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==0){ // checking whether swich1 is pressed
		if(flag==0) flag=1;
		else if(flag==1)flag=2;
	}else{
		flag=0;
	}
	if(flag==2)return 1;
	else return 0;
}


/*
 * Function Name:- detectKey2Press
 *
 * Input:			No arguments needed
 *
 * Ouptut:-			returns 1 if the switch2 is kept pressed for 2 timer interupts else returns 0
 *
 * Logic:-			flag1 is a variable which is used to know how many interupts it has been found
 * 					pressed and its count is used to return ouput
 *
 * Example Call:	detectKey2Press()
 *
 */

unsigned char detectKey2Press()
{
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)==0){// checking whether swich2 is pressed
		if(flag1==0)flag1=1;
		else if(flag1==1)flag1=2;
	}else{
		flag1=0;
	}
	if(flag1==2)return 1;
	else return 0;
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


int main(void)
{
	//setting up hardware
	setup();
	ledPinConfig();
	switchPinConfig();
	timerConfig();

	//setting up Timer for interrupt
	ui32Period = (SysCtlClockGet() / 100)/2;
	flag=0;
	flag1=0;
	//state='I';
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
	while(1)
	{
	}
}


/*
 * Function Name:- Timer0IntHandler
 *
 * Input:			No arguments needed
 *
 * Ouptut:-			perform the state logic
 *
 * Logic:-			Detects the presses and changes the states and calls the corresponding functions
 *
 * Example Call:	Timer0IntHandler()
 *
 */


void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // clearing the interrupt
	// Read the current state of the GPIO pin and
	// write back the opposite state
	/*if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
	}else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
	}
	if(state=='I'){
		int32_t is_off = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
		if(!is_off){
			state='P';
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
			if (ui8LED == 8)
					{
						ui8LED = 2;
					}
					else
					{
						ui8LED = ui8LED*2;
					}
			 }
	}else if(state=='P'){
		int32_t is_off = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
		if(!is_off){
			state='R';
		 }else{
			 state='I';
			 flag=0;
			 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
		 }
	}else{
		int32_t is_off = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
		if(is_off){
			 state='I';
			 flag=0;
			 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
		 }
	}
	int32_t is_off = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4); // reading the switch1
	if(!is_off){ // checking whether it is off
		if(detectKeyPress()!='1'){ //if it is off for the firt time
			flag='1'; // chaning the next value and glowing blub
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
			switch(ui8LED) {
				case 8:ui8LED = 4; break;
				case 4:ui8LED = 2; break;
				case 2:ui8LED = 8; break;
			}
		 }
	}else{
		// setting the flag to zero
		flag=0;
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
	}
	is_off = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
	if(!is_off){
		if(detectKey2Press()!='1'){
			flag1='1';
			sw2Status = sw2Status+1;
		}
	}else{
		flag1=0;
	}
	*/
	if(detectKeyPress()){// if key1 has pressed
		if(!ledOn){// if led is not one. one it and change color of next press led
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
			ledOn=true;
			switch(ui8LED) {
			case 8:ui8LED = 4; break;
			case 4:ui8LED = 2; break;
			case 2:ui8LED = 8; break;
			}
		}

	}else{
		// not glowing led
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
		ledOn=false;
	}
	if(detectKey2Press()){// if key2 has pressed
		if(!counting){//if it is for the first time increasing
			sw2Status = sw2Status+1;
			counting = true;
		}
	}else{
		counting = false;
	}
}
