/*
 *
 * TeamId :			#Don't Know
 *
 * Author List:		Pranay Dhondi		(120050054)
 * 					Akhil Surya Vamshi	(120050067)
 *
 * Filename:		lab-3.c
 *
 * Theme:			None
 *
 * Functions:		autoMode(), Increase(),Decrease(),
 * 					detectKeyPress(),detectKey2Press(),
 * 					Timer0IntHandler(void),timerConfig(void),
 * 					main(void)
 *
 * Global Variables: ui8rAdjust,ui8gAdjust,ui8bAdjust,
 * 					curr,state,flag,flag1,ui32Load,ui32Period,
 * 					pressCount1,delayCount,pressedBefore1,
 * 					pressedBefore2,longPress1,pressedFor1
 *
*/


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/tm4c123gh6pm.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

// Setting base frequency to control the servo
#define PWM_FREQUENCY 55 //55 hertz

volatile uint8_t ui8rAdjust,ui8gAdjust,ui8bAdjust; //ui8xAdjust -> Variable representing the brigtness/intensity value of 'x' color led light
unsigned char curr = 'r'; // curr -> represents the color of led light decreasing
unsigned char state='a'; // state-> represnts the state of the system
unsigned char flag=0,flag1=0; //flag to indicate the presss status of button1, button respectively
volatile uint32_t ui32Load; //This is the unit period of pwn
uint32_t ui32Period; //Indicates the time period of the clock_A used
uint32_t pressCount1=0; // how many times swich has pressed
uint32_t delayCount = 100000;
bool pressedBefore1=false, pressedBefore2=false; // whenther button pressed recogonized as real press
bool longPress1 = false;  // includes this is a normal press
uint32_t pressedFor1 = 0; // this is include very long press


/*
 * Function Name:- autoMode
 *
 * Input:			Nothing
 *
 * Ouptu:-			Analyzes the state in autoMode(as mentioned in question)
 * 					and does corresponding funtion
 *
 * Logic:-			Defined the global variables which will be called at the timeout
 * 					which will update the states of function. If current state is
 * 					'x' which means the 'x'th led intensity is drecreasing so it changes
 * 					state to corresponding 'y' state so that they form a circle of colors
 * 					as mentioned in program
 *
 * Example Call:	autoMode()
 *
 */
void autoMode() {
	//checking the state in Automode
	if(curr=='r'){ //if the state is 'r' which means red led value is decreasing
		ui8rAdjust--; //decresing its brighness
		ui8gAdjust++; //increasing reen brightness to follow color circle
		if(ui8rAdjust==0){
			curr='g'; // switching state
			ui8gAdjust=254;
			ui8bAdjust=0;
			PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false); // Disabling the output pin of PWM to the corresponding pin
			//PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 0);
			PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);  // Enabling the output pin of PWM to the corresponding pin
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8gAdjust * ui32Load / 1000); // setting value of led value
		}else{
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8rAdjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8gAdjust * ui32Load / 1000);
		}
	}else if(curr=='g'){//same for state 'g'
		ui8gAdjust--;
		ui8bAdjust++;
		if(ui8gAdjust==0){
			curr='b';
			ui8bAdjust=254;
			ui8rAdjust=0;
			PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, false);
			//PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 0);
			PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8bAdjust * ui32Load / 1000);
		}else{
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8gAdjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8bAdjust * ui32Load / 1000);
		}

	}else{// same logic fro state 'r'
		ui8bAdjust--;
		ui8rAdjust++;
		if(ui8bAdjust==0){
			curr='r';
			ui8rAdjust=254;
			ui8gAdjust=0;
			PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
			//PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 0);
			PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8rAdjust * ui32Load / 1000);
		}else{
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8rAdjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8bAdjust * ui32Load / 1000);
		}
	}
}

/*
 * Function Name:- Increase
 *
 * Input:			No arguments needed
 *
 * Ouptut:-			If the system is in automatic mode, it increases the frequency of switching the
 * 					colors and if it is other mode then it increases the intensity of the
 * 					led in corresponding state
 *
 * Logic:-			If system is in automatic mode, the delay between two consecutive call for autoMode()
 * 					will be decreased in other states used PWM to increase its intensity value
 *
 * Example Call:	Increase()
 *
 */

void Increase(){
	if(state=='a'){
		delayCount+=1000; //delaybetween two calls for autoMode
		if(delayCount > 1000000){
			delayCount=1000000;
		}
	}else if(state=='r'){// PWM to increase the red led intensity
		ui8rAdjust=ui8rAdjust+6;
		if (ui8rAdjust > 254)
		{
			ui8rAdjust = 254;
		}
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8rAdjust * ui32Load / 1000);
	}else if(state == 'g'){/ PWM to increase the green led intensity
		ui8gAdjust+=6;
		if (ui8gAdjust > 254)
		{
			ui8gAdjust = 254;
		}
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8gAdjust * ui32Load / 1000);
	}else if(state == 'b'){/ PWM to increase the blue led intensity
		ui8bAdjust+=6;
		if (ui8bAdjust > 254)
		{
			ui8bAdjust = 254;
		}
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8bAdjust * ui32Load / 1000);
	}
}

/*
 * Function Name:- Decrease
 *
 * Input:			No arguments needed
 *
 * Ouptut:-			If the system is in automatic mode, it decreases the frequency of switching the
 * 					colors and if it is other mode then it decreases the intensity of the
 * 					led in corresponding state
 *
 * Logic:-			If system is in automatic mode, the delay between two consecutive call for autoMode()
 * 					will be increased in other states used PWM to decrease its intensity value
 *
 * Example Call:	Decrease()
 *
 */

void Decrease(){
	if(state=='a'){
		delayCount-=1000; //delaybetween two calls reduced
		if(delayCount < 1000){
			delayCount=1000;
		}
	}else if(state=='r'){// PWM to decrease the red led intensity
		ui8rAdjust-=6;
		if (ui8rAdjust < 1)
		{
			ui8rAdjust = 1;
		}
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8rAdjust * ui32Load / 1000);
	}else if(state == 'g'){
		ui8gAdjust-=6;// PWM to decrease the green led intensity
		if (ui8gAdjust < 1)
		{
			ui8gAdjust = 1;
		}
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8gAdjust * ui32Load / 1000);
	}else if(state == 'b'){
		ui8bAdjust-=6;// PWM to decrease the blue led intensity
		if (ui8bAdjust < 1)
		{
			ui8bAdjust = 1;
		}
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8bAdjust * ui32Load / 1000);
	}
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
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==0){ // checking whether the button 1 is pressed
		if(flag==0) flag=1; // flag ->measures the last k consutive presses
		else if(flag==1)flag=2;
	}else{
		flag=0;
	}
	if(flag==2)return 1; //if consecutive presses are 2 returning it as 1
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
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)==0){// checking whether the button 2 is pressed
		if(flag1==0)flag1=1; // flag1 ->measures the last 'flag1' consutive presses of button2
		else if(flag1==1)flag1=2;
	}else{
		flag1=0;
	}
	if(flag1==2)return 1;
	else return 0;
}


/*
 * Function Name:- Timer0IntHandler
 *
 * Input:			No arguments needed
 *
 * Ouptut:-			perform the state logic
 *
 * Logic:-			Detects the presses and changes the states and calls the corresponding funcitons
 *
 * Example Call:	Timer0IntHandler()
 *
 */

void Timer0IntHandler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // clearing the interupt
	unsigned char key1= detectKeyPress(); // knowing the button is pressed
	unsigned char key2 = detectKey2Press(); // status of button2 i.e., pressed or not
	if (!pressedBefore2 && key2 == 1) { // if it is not a long press then just run state in auto state
		pressedBefore2 = true;
	} else if (pressedBefore2 && key2 == 1) {
		if (!pressedBefore1 && key1 == 1) {
			pressedBefore1 = true;
		} else if (pressedBefore1 && key1 == 1) {
			pressedFor1++;
			if (pressedFor1 > 100) {
				longPress1 = true;
			}
		} else if (pressedBefore1 && key1 ==0) {
			pressCount1++;
			pressedBefore1 = false;
			pressedFor1= 0;
		}
	} else if (pressedBefore2 && key2 == 0) {
		if (longPress1) {
			state='g';
			ui8gAdjust=254;
			PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);
			PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
			PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
			delayCount = 100000;
		} else if (pressCount1 == 1) {
			PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
			PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
			PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, false);
			ui8rAdjust=254;
			state='r';
			delayCount = 100000;
		} else if (pressCount1 == 2) {
			state='b';
			PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);
			PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
			PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, false);
			ui8bAdjust=254;
			delayCount = 100000;
		} else if (pressCount1 == 0) {
			Increase();
		}
		longPress1 = false;
		pressedBefore2 = false;
		pressCount1 = 0;


	} else {
		if (!pressedBefore1 && key1==1) {
			pressedBefore1 = true;

		} else if (pressedBefore1 && !key1) {
			Decrease();
			pressedBefore1 = false;
		}
	}






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

	volatile uint32_t ui32PWMClock;

	ui8rAdjust = 255,ui8gAdjust=0,ui8bAdjust=0;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // Setting up clock
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64); // setting up PWN clock to systemclock/64
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // Enabling the pwm1

	//enabling and marking ports/pins
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_2);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);

	// enabling timer
	timerConfig();
	ui32Period = (SysCtlClockGet() / 100)/2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);

	// removing lock associated with button2
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);


	//setting up pwm and pwn pins
	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8rAdjust * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);

	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8gAdjust * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8bAdjust * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

	// starting states
	while(1)
	{


		if(state=='a')autoMode();
		//Timer0IntHandler();
		SysCtlDelay(delayCount);
	}
}
