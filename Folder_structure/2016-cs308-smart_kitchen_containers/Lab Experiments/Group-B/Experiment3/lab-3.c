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

#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

// Different button states
#define IDLE 0
#define PRESS 1
#define RELEASE 2

// Setting base frequency to control the servo
#define PWM_FREQUENCY 55 //55 hertz

volatile uint8_t ui8rAdjust,ui8gAdjust,ui8bAdjust,ui8Adjust;
volatile unsigned char curr = 'r';
volatile uint32_t ui32Load;

volatile uint8_t speedCount = 1;
volatile uint8_t pressCount1 = 0, pressCount2 = 0;

volatile uint8_t sw1State=IDLE, sw2State=IDLE;
volatile uint8_t sw2LongPress = 0, sw1ClickCount = 0;
volatile unsigned char sw1Flag=0, sw2Flag=0;

volatile uint8_t mode = 0;

void autoMode() {
	if(curr=='r'){
		if(ui8rAdjust-speedCount > 1) ui8rAdjust-=speedCount;
		else ui8rAdjust = 1;
		if(ui8gAdjust+speedCount < 254) ui8gAdjust+=speedCount;
		else ui8gAdjust = 254;

		if(ui8rAdjust==1){
			curr='g';
			ui8gAdjust=254;
			ui8bAdjust=1;
			PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);
			PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8gAdjust * ui32Load / 1000);
		}else{
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8rAdjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8gAdjust * ui32Load / 1000);
		}
	}else if(curr=='g'){
		if(ui8gAdjust-speedCount > 1) ui8gAdjust-=speedCount;
		else ui8gAdjust = 1;
		if(ui8bAdjust+speedCount < 254) ui8bAdjust+=speedCount;
		else ui8bAdjust = 254;

		if(ui8gAdjust==1){
			curr='b';
			ui8bAdjust=254;
			ui8rAdjust=1;
			PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, false);
			PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8bAdjust * ui32Load / 1000);
		}else{
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8gAdjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8bAdjust * ui32Load / 1000);
		}

	}else{
		if(ui8bAdjust-speedCount > 1) ui8bAdjust-=speedCount;
		else ui8bAdjust = 1;
		if(ui8rAdjust+speedCount < 254) ui8rAdjust+=speedCount;
		else ui8rAdjust = 254;

		if(ui8bAdjust==1){
			curr='r';
			ui8rAdjust=254;
			ui8gAdjust=1;
			PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
			PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8rAdjust * ui32Load / 1000);
		}else{
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8rAdjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8bAdjust * ui32Load / 1000);
		}
	}

}

void manualMode(){
	if(mode == 1){
		PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
		PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
		PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, false);
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
	}else if(mode == 2){
		PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);
		PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
		PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, false);
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust * ui32Load / 1000);
	}else if(mode == 3){
		PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);
		PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
		PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust * ui32Load / 1000);
	}
}

void setup(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

void led_pwm_pin_config(){
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_2);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);
}

void switch_pin_config(void) {
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
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

int main(void)
{
	ui8rAdjust = 254;
	ui8gAdjust=1;
	ui8bAdjust=1;


	ui8Adjust = 128;

	setup();
	led_pwm_pin_config();

	timer_pin_config();
	interrupt_config();

	switch_pin_config();

	uint32_t ui32PWMClock;
	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;

	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);

	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8rAdjust * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8bAdjust * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8gAdjust * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);

	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

	TimerEnable(TIMER0_BASE, TIMER_A);

	while(1)
	{
		if(mode==0) autoMode();
		else manualMode();

		SysCtlDelay(100000);
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

void setMode(uint8_t m){
	mode = m;
	ui8Adjust = 128;
}

void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if(detectSwitch1KeyPress()==1){
		if(mode==0) {
			if(speedCount<255) speedCount++;
		}else {
			if(ui8Adjust<255) ui8Adjust++;
		}

		if(sw2LongPress==1){ // Only when sw2 is long pressed
			if(pressCount1<60) pressCount1++;
			else {
				setMode(3); //sw1 is long pressed so go to manual mode 3
				sw1ClickCount = 0;
			}
		}
	}else{
		if(sw2LongPress==1 && pressCount1 < 60 && pressCount1 > 0) {
			sw1ClickCount+=1;
			if(sw1ClickCount==1){
				setMode(1); //go to manual mode 1
			}else {
				setMode(2); //go to manual mode 2
			}
		}
		if(sw2LongPress==0) sw1ClickCount = 0;
		pressCount1 = 0;
	}

	if(detectSwitch2KeyPress()==1){
		if(mode==0) {
			if(speedCount>0) speedCount--;
		}else {
			if(ui8Adjust>1) ui8Adjust--;
		}

		if(pressCount2<60) pressCount2++;
		else {
			sw2LongPress = 1; //sw2 long press flag set to 1
		}
	}else{
		pressCount2 = 0;
		sw2LongPress = 0;
	}
}
