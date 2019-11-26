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
#include "SysTick.h"
#include "timer.h"

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



static char SendData[] = {0xAA,0x55,0xC3,0x3C,0x07,0x01};

static char complete[] = {0xAA,0x55,0xC3,0x3C,0x07,0x01, 0x01,0x01, 0x01, 0x01, 0x01, 0x01, '\0'};
// hay que mejorar la uart en el sentido de que no me deja mandar 0 porque lo considera terminador


static char data[] = {0x0,0x0,0x1,0x0,0x2,0x0}; // recibe un uint16 que son dos bytes

static char data2[] = {0x1,0x0,0x1,0x0,0x3,0x0}; // recibe un uint16 que son dos bytes

static bool toggle_msg = false;

tim_id_t timerUpdatePos;

static char rxBuffer[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14};
static int index = 0;
void append(char c){
	rxBuffer[index%sizeof(rxBuffer)] = c;
	index++;
	if(index == 12){
		//
	}
}


void charListener(char c){
	append(c);
}

void senddata2thingspeak(){
    //sendWord(SendData); //esto funca
//    sendWord(complete); //esto funca
	sendCharArray(SendData,sizeof(SendData));

	if(toggle_msg){
		sendCharArray(data,sizeof(data));
	}else{
		sendCharArray(data2,sizeof(data2));
	}
	toggle_msg = !toggle_msg;

}

void App_Init (void)
{
	uart_cfg_t config;
	config.baudrate = 1200;
    gpioMode(PIN_LED_BLUE, OUTPUT);
    uartInit(config);
    setOnNewCharListener(charListener);
	timerInit();
	timerUpdatePos = timerGetId();
	timerStart(timerUpdatePos, TIMER_MS2TICKS(10000), TIM_MODE_PERIODIC, senddata2thingspeak);
}



/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
//    updateWord(); // actualiza la cola de uart, flush de queue
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
