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
#include "Lector.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
__ISR__ PORTC_IRQHandler (void){

	if( ((PORTC -> PCR[ PIN2NUM(EN)]) & (PORT_PCR_ISF_MASK)) == (PORT_PCR_ISF_MASK))
	{
		set_Enable(gpioRead(EN));
		PORT_ClearInterruptFlag(EN);


	}
	else if (((PORTC -> PCR[ PIN2NUM(CLK)]) & (PORT_PCR_ISF_MASK)) ==PORT_PCR_ISF_MASK)
	{
		get_Data(gpioRead(DATA));
		PORT_ClearInterruptFlag(CLK);
	}

}


/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    gpioMode(PIN_LED_BLUE, OUTPUT);

    gpioWrite(PIN_LED_BLUE, HIGH);

    gpioMode(EN, INPUT);
    gpioMode(DATA, INPUT);
    gpioMode(CLK, INPUT);

    gpioIRQC(EN, INTERRUPT_BOTH_EDGES);
    gpioIRQC(CLK, INTERRUPT_FALLING_EDGE);

    NVIC_EnableIRQ(PORTC_IRQn);




}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	// Loop
	int * a = get_ID();
	if (a != NULL)
	{
		gpioWrite(PIN_LED_BLUE, LOW);
	}
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
