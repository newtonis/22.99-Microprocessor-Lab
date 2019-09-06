/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"
#include "gpio.h"
#include "SysTick.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void delayLoop(uint32_t veces);
static bool test;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
__ISR__ SysTick_Handler (void)
{
	gpioToggle(PIN_TP);
	static int veces = 0;
	if(veces == 5)
	{
		gpioToggle(PIN_LED_BLUE);
		gpioToggle(PIN_CONTROL);
		veces = 0;
	}
	veces++;
	test = !test;



}

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{

    gpioMode(PIN_LED_BLUE, OUTPUT);
    gpioMode(PIN_SW3, INPUT);
    gpioMode(PIN_CONTROL, OUTPUT);
    gpioMode(PIN_TP, OUTPUT);

    test = LOW;

    gpioWrite(PIN_TP, LOW);

    gpioIRQC(PIN_SW3, INTERRUPT_RISING_EDGE);
    NVIC_EnableIRQ(PORTA_IRQn);

    NVIC_EnableIRQ(SysTick_IRQn);
    SysTick_Init();


}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	if(test == HIGH)
	{
		gpioToggle(PIN_TP);
		test = 0;
	}
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/




/*******************************************************************************
 ******************************************************************************/
