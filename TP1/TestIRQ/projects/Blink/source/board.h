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
#define PIN_LED_RED     PORTNUM2PIN(PB,22) // PTB22
#define PIN_LED_GREEN   PORTNUM2PIN(PE,26) // PTE26
#define PIN_LED_BLUE    PORTNUM2PIN(PB,21) // PTB21

#define TEST_PIN		PORTNUM2PIN(PE,24)

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
#define RSwitch	PORTNUM2PIN(PC,3)
#define RCHB	PORTNUM2PIN(PC,2)
#define RCHA	PORTNUM2PIN(PA,2)

//Lector Pins
#define EN		PORTNUM2PIN(PD,1)
#define DATA 	PORTNUM2PIN(PD,2)
#define CLK 	PORTNUM2PIN(PD,3)

/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
