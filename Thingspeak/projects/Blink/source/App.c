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
#include "uart.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void delayLoop(uint32_t veces);


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */

void charListener(char c){

}

void App_Init (void)
{
	uart_cfg_t config;
	config.baudrate = 9600;
    gpioMode(PIN_LED_BLUE, OUTPUT);
    uartInit(config);
    setOnNewCharListener(charListener);
}


/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
    //delayLoop(4000000UL);
    //gpioToggle(PIN_LED_BLUE);
    //sendWord("hola amigos xd");
    updateWord(); // actualiza la cola de uart, flush de queue
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void delayLoop(uint32_t veces)
{
    while (veces--);
}


/*******************************************************************************
 ******************************************************************************/
