/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// On Board User LEDs
#define PIN_LED_RED     // ???
#define PIN_LED_GREEN   // ???
#define PIN_LED_BLUE    PORTNUM2PIN(PB,21) // PTB21

#define LED_ACTIVE      LOW

// On Board User Switches
#define PIN_SW2         PORTNUM2PIN(PC,6) // PTC6
#define PIN_SW3			PORTNUM2PIN(PA,4) // PTA4

#define SW_ACTIVE       // ???
#define SW_INPUT_TYPE   // ???

// Display Segments
#define SEG_A	PORTNUM2PIN(PC,5)
#define SEG_B	PORTNUM2PIN(PC,7)
#define SEG_C	PORTNUM2PIN(PC,0)
#define SEG_D	PORTNUM2PIN(PC,9)
#define SEG_E	PORTNUM2PIN(PC,8)
#define SEG_F	PORTNUM2PIN(PC,1)
#define SEG_G	PORTNUM2PIN(PB,19)
#define SEG_DP	PORTNUM2PIN(PB,18)

// Display Selector
#define	SEL0	PORTNUM2PIN(PC,17)
#define SEL1	PORTNUM2PIN(PC,16)

// Status LED's
#define STAT0	PORTNUM2PIN(PA,1)
#define STAT1	PORTNUM2PIN(PB,9)

// Encoder Pins
#define RCHA	PORTNUM2PIN(PC,3)
#define RCHB	PORTNUM2PIN(PC,2)
#define RSwitch	PORTNUM2PIN(PA,2)

/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
