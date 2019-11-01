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
int demodulado = 1;
long long contador = 0;

char holaHi[10000];
int i = 0;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void test(void);
void callback (void);
void demodulador(void);

void testfunc(char c){
	char str[3];
	str[0] = c;
	str[1] = '\0';

	//delayLoop(400000UL);

	sendWord(str);

}

void testfunc2(char c){
	holaHi[i] = c;
	i ++;

	Modulador_sendChar(c);
	demodulado = 0;

	contador = 0;
}
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	uart_cfg_t config;
	config.baudrate = 1200;
	uartInit(config);
	setOnNewCharListener(testfunc2);
	//setOnNewCharListener(testfunc);

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
	if (demodulado == 1){
		if (contador < 100){
			contador ++;
			gpioWrite(PIN_LED_BLUE, (contador % 5000 >= 5000/2) ? HIGH : LOW );
		}else{
			updateWord();
			gpioWrite(PIN_LED_BLUE, HIGH);
		}
	}else{
		gpioWrite(PIN_LED_BLUE, LOW);
	}
	if(isDataReady() == true){
		FSKdemodulate();
	}


	//delayLoop(400UL);
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


void demodulador(void)
{

	char b = get_Msg();
	int a = 0;

	char str[3];
	str[0] = b;
	str[1] = '\0';
	sendWord(str);

	demodulado = 1;
	contador = 0;
}

/*******************************************************************************
 ******************************************************************************/
