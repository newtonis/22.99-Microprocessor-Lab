/*
 * InternalControl.c
 *
 *  Created on: 5 Sep 2019
 *      Author: Marcelo
 */

#include "InternalControl.h"
#include "board.h"

static bool OK_sw = 0;
static bool CANCEL_sw = 0;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void isr_swOK(void);
void isr_swCANCEL(void);

void PortInit(void){
    gpioMode(PIN_SW2, INPUT_PULLUP);
    gpioMode(PIN_SW3, INPUT);

    gpioIRQ(PIN_SW2, GPIO_IRQ_MODE_RISING_EDGE, isr_swOK);
    gpioIRQ(PIN_SW3, GPIO_IRQ_MODE_RISING_EDGE, isr_swCANCEL);

    NVIC_EnableIRQ(PORTA_IRQn);
    NVIC_EnableIRQ(PORTC_IRQn);

    gpioMode(PIN_LED_RED, OUTPUT);
    gpioMode(PIN_LED_BLUE, OUTPUT);
    gpioMode(PIN_LED_GREEN, OUTPUT);

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
