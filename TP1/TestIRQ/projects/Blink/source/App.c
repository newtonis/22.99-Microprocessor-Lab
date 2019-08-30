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
#include "DisplayBoard.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static int test_num[ID_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
static int test_pin[PIN_LEN] = {1, 2, 3, 4, 5};
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
__ISR__ PORTA_IRQHandler (void){
	PORT_ClearInterruptFlag(PIN_SW3);
	PORT_ClearInterruptFlag(RSwitch);
	gpioToggle(PIN_LED_BLUE);

}

__ISR__ PORTC_IRQHandler (void){
	PORT_ClearInterruptFlag(RCHA);
	PORT_ClearInterruptFlag(RCHB);


}

__ISR__ SysTick_Handler (void){
	gpioToggle(PIN_LED_BLUE);
	DispShowPIN(test_pin);
}

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    gpioMode(PIN_LED_BLUE, OUTPUT);
    gpioMode(PIN_SW3, INPUT);

    gpioMode(RCHA, INPUT);
    gpioMode(RCHB, INPUT);
    gpioMode(RSwitch, INPUT);

    gpioIRQC(PIN_SW3, INTERRUPT_RISING_EDGE);
    gpioIRQC(RSwitch, INTERRUPT_FALLING_EDGE);
    NVIC_EnableIRQ(PORTA_IRQn);

    gpioIRQC(RCHA, INTERRUPT_FALLING_EDGE);
    gpioIRQC(RCHB, INTERRUPT_FALLING_EDGE);
    NVIC_EnableIRQ(PORTC_IRQn);

    NVIC_EnableIRQ(SysTick_IRQn);
    SysTick_Init();

    DispBoard_Init();
    Status_Write(1);
    Status_Write(2);
    Status_Write(3);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	// Loop
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
