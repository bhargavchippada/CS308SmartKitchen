#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

// Different button states
#define IDLE 0
#define PRESS 1
#define RELEASE 2

/*
 ------ Global Variable Declaration
 */

uint8_t sw2Status;
uint8_t sw1State, sw2State;
unsigned char sw1Flag, sw2Flag;
uint8_t ui8LED;

/*

 * Function Name: setup()

 * Description: Set crystal frequency and enable GPIO Peripherals

 * Example Call: setup();

 */
void setup(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*

 * Function Name: led_pin_config()

 * Description: Set PORTF Pin 1, Pin 2, Pin 3 as output. On this pin Red, Blue and Green LEDs are connected.

 * Example Call: led_pin_config();

 */

void led_pin_config(void) {
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

/*

 * Function Name: switchPinConfig()

 * Description: Set PORTF Pin 0 (SW2) and Pin 4 (SW1) as input. Note that Pin 0 is locked.

 * Example Call: switchPinConfig();

 */
void switch_pin_config(void) {
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F = 0x4C4F434BU;
	CR_F = GPIO_PIN_0 | GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_DIR_MODE_IN);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD_WPU);
}

void timer_pin_config(void) {
	uint32_t ui32Period;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	ui32Period = (SysCtlClockGet() / 100); // 10 ms interval or 100 Hz
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);
}

void interrupt_config(void) {
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
}

int main(void) {

	setup();
	led_pin_config();
	switch_pin_config();
	timer_pin_config();
	interrupt_config();

	sw1State = IDLE;
	sw2State = IDLE;
	sw2Status = 0;
	sw1Flag = 0;
	sw2Flag = 0;
	ui8LED = 2;

	TimerEnable(TIMER0_BASE, TIMER_A);

	while (1) {
	}
}

unsigned char detectSwitch1KeyPress(void) {
	uint8_t swPinRead = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);

	switch (sw1State) {
	case IDLE:
		if (swPinRead == 0x00) {
			sw1State = PRESS; // enter press state
		}
		break;
	case PRESS:
		if (swPinRead == 0x00) {
			sw1State = RELEASE; // enter release state
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, ui8LED);
			return 1;
		} else if (swPinRead == 0x10) {
			sw1State = IDLE; // enter idle state
		}
		break;
	case RELEASE:
		if (swPinRead == 0x00) {
			return 1; // remain in this state
		} else if (swPinRead == 0x10) {
			sw1State = IDLE; // enter idle state
			// Turn off the LED
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);

			//Change LED
			if (ui8LED == 2) {
				ui8LED = 8;
			} else if (ui8LED == 8) {
				ui8LED = 4;
			} else if (ui8LED == 4) {
				ui8LED = 2;
			}
		}
		break;
	}

	return 0;
}

unsigned char detectSwitch2KeyPress(void) {
	uint8_t swPinRead = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);

	switch (sw2State) {
	case IDLE:
		if (swPinRead == 0x00) {
			sw2State = PRESS; // enter press state
		}
		break;
	case PRESS:
		if (swPinRead == 0x00) {
			sw2State = RELEASE; // enter release state
			sw2Status++;
			return 1;
		} else if (swPinRead == 0x01) {
			sw2State = IDLE; // enter idle state
		}
		break;
	case RELEASE:
		if (swPinRead == 0x00) {
			return 1; // remain in this state
		} else if (swPinRead == 0x01) {
			sw2State = IDLE; // enter idle state
		}
		break;
	}

	return 0;
}

void Timer0IntHandler(void) {
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	sw1Flag = detectSwitch1KeyPress();
	sw2Flag = detectSwitch2KeyPress();
}
