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
void App_Init (void)
{
	gpioMode(PIN_SW3, INPUT);
    gpioMode(LED_YELLOW, OUTPUT);
    gpioMode(PIN_LED_RED, OUTPUT);
    gpioWrite(LED_YELLOW, LOW);
    gpioWrite(PIN_LED_RED, HIGH);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	static int timer = 0;
	static int estado = HIGH;
	static int baliza = LOW; // Inicialmente apagado
	int estado_viejo = estado;
	timer++;
	estado = gpioRead(PIN_SW3);
	if((estado_viejo == HIGH)&&(estado == LOW)){
		if(baliza == LOW){
			baliza = HIGH;
		}else{
			baliza = LOW;
		}
	}

	delayLoop(144000UL); // 10ms

	switch(baliza){
	case LOW:
		if(gpioRead(PIN_LED_RED) == LOW){
		    gpioWrite(LED_YELLOW, LOW);
		    gpioWrite(PIN_LED_RED, HIGH);
		}
		break;
	case HIGH:
		if(timer % 100 == 0){
			gpioToggle(LED_YELLOW);
		}
		if(gpioRead(PIN_LED_RED) == HIGH){
			gpioWrite(PIN_LED_RED, LOW);
		}
		break;
	default:
		// Nada
		break;
	}
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
