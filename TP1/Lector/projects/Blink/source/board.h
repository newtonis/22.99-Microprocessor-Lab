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


#define SW_ACTIVE       // ???
#define SW_INPUT_TYPE   // ???



//Lector Pins
#define EN		PORTNUM2PIN(PC,7)
#define DATA 	PORTNUM2PIN(PC,5)
#define CLK 	PORTNUM2PIN(PC,9)

/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
