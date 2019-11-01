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
#define PIN_SW2         // ???
#define PIN_SW3         // ???

#define SW_ACTIVE       // ???
#define SW_INPUT_TYPE   // ???

#define PIN_PRUEBA PORTNUM2PIN(PB,9)
#define PIN_INPUT_PRUEBA PORTNUM2PIN(PA,4)

#define PIN_FTM_TEST_OVF			PORTNUM2PIN(PD,1) // PTD1

#define PIN_OUTPUT_CAPTURE PORTNUM2PIN(PC,1)

#define PIN_GPIO_TEST PORTNUM2PIN(PC,8)
#define PIN_SCK PORTNUM2PIN(PC,8)

#define PIN_INPUT_CAPTURE PORTNUM2PIN(PC,9)

/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
