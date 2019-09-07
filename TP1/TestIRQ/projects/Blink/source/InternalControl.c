/*
 * InternalControl.c
 *
 *  Created on: 5 Sep 2019
 *      Author: Marcelo
 */

#include "InternalControl.h"
#include "board.h"
#include "timer.h"

static bool OK_sw = 0;
static bool CANCEL_sw = 0;

static int internal_sw_event = 0;

static tim_id_t timerSw;

static void (*callback)();

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void isr_swOK(void);
void isr_swCANCEL(void);
void readSwitchInterface(void);
bool PortGetOK_Status(void);
void PortClearOK_Status(void);
bool PortGetCANCEL_Status(void);
void PortClearCANCEL_Status(void);

/*******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 ******************************************************************************/

void internalControlInit(void (*funcallback)(void)){
    gpioMode(PIN_SW2, INPUT_PULLUP);
    gpioMode(PIN_SW3, INPUT);

    gpioIRQ(PIN_SW2, GPIO_IRQ_MODE_RISING_EDGE, isr_swOK);
    gpioIRQ(PIN_SW3, GPIO_IRQ_MODE_RISING_EDGE, isr_swCANCEL);

    NVIC_EnableIRQ(PORTA_IRQn);
    NVIC_EnableIRQ(PORTC_IRQn);

    gpioMode(PIN_LED_RED, OUTPUT);
    gpioMode(PIN_LED_BLUE, OUTPUT);
    gpioMode(PIN_LED_GREEN, OUTPUT);

    callback = funcallback;

    timerSw = timerGetId();
    timerStart(timerSw, TIMER_MS2TICKS(100), TIM_MODE_PERIODIC, readSwitchInterface);

    RGBIndicator(BLUE_INDICATOR);
}

void isr_swOK(void){
	OK_sw = 1;
}

void isr_swCANCEL(void){
	CANCEL_sw = 1;
}

bool PortGetOK_Status(void){
	return OK_sw;
}
void PortClearOK_Status(void){
	OK_sw = 0;
}

bool PortGetCANCEL_Status(void){
	return CANCEL_sw;
}

void PortClearCANCEL_Status(void){
	CANCEL_sw = 0;
}

void readSwitchInterface(void){
	if(PortGetOK_Status()){
		internal_sw_event = OK_EVENT;
		callback();
		PortClearOK_Status();
	}else if(PortGetCANCEL_Status()){
		internal_sw_event = CANCEL_EVENT;
		callback();
		PortClearCANCEL_Status();
	}
}

int internalControlGetEvent(void){
	if(internal_sw_event == OK_EVENT){
		internal_sw_event = NONE;
		return OK_EVENT;
	}else if(internal_sw_event == CANCEL_EVENT){
		internal_sw_event = NONE;
		return CANCEL_EVENT;
	}
	return NONE;
}

void RGBIndicator(int led_color){
	switch(led_color){
	case RED_INDICATOR:
	    gpioWrite(PIN_LED_RED, LED_ACTIVE);
	    gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);
	    gpioWrite(PIN_LED_GREEN, !LED_ACTIVE);
		break;
	case BLUE_INDICATOR:
	    gpioWrite(PIN_LED_RED, !LED_ACTIVE);
	    gpioWrite(PIN_LED_BLUE, LED_ACTIVE);
	    gpioWrite(PIN_LED_GREEN, !LED_ACTIVE);
		break;
	case GREEN_INDICATOR:
	    gpioWrite(PIN_LED_RED, !LED_ACTIVE);
	    gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);
	    gpioWrite(PIN_LED_GREEN, LED_ACTIVE);
		break;
	}
}

/*******************************************************************************
 ******************************************************************************/
