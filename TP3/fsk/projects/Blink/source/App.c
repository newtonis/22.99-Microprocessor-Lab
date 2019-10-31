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
#include "Modulador.h"
#include "demodulatorFSK.h"
#include "timer.h"
#include "uart.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

uint8_t a = 0;
tim_id_t timer;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void test(void);
void callback (void);
void demodulador(void);


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	uart_cfg_t config;
	config.baudrate = 9600;
	uartInit(config);
	setOnNewCharListener(Modulador_sendChar);

	timerInit();
	timer = timerGetId();
	gpioMode(PIN_LED_BLUE, OUTPUT);
	//gpioMode(PIN_TP, OUTPUT);
	//gpioWrite (PIN_TP, false);
    Modulador_init(test);
    FSKdem_init(demodulador);
    //timerStart(timer, TIMER_MS2TICKS(5000), TIM_MODE_PERIODIC, callback);

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	updateWord();
	delayLoop(4000UL);
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void test(void)
{
	a = 1; // avisa que termino de enviar la señal modulada por callback
}


/*
void callback (void)
{
	Modulador_sendChar('k');
}
*/

void demodulador(void)
{

	char b = get_Msg();
	int a = 0;

	char str[3];
	str[0] = b;
	str[1] = '\0';
	sendWord(str);
}

/*******************************************************************************
 ******************************************************************************/
