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
#include "ftm.h"
#include "DecodeBits.h"
#include "CMP.h"
#include "uart.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


static bool testStream[STAND_LEN] =  {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1};
static bool testStream2[STAND_LEN] = {0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0};
static bool dummyStream[STAND_LEN] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static uint8_t a = 0;



/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void test(void);

void testDecode(void);

void testfunc(char c);

void testfunc2(char c);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void incrementOvfCnt(void){
	setCntOvf();
}

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	ftmInit(incrementOvfCnt);
    CMP_init(0);
    Modulador_init(getDutyAddress(), test);
    Decoder_init(getMedAddress(), testfunc);

	uart_cfg_t config;
	config.baudrate = 1200;
	uartInit(config);
	setOnNewCharListener(testfunc2);

    //Modulador_sendStream(dummyStream);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	updateWord();
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void test(void)
{
/*
	// avisa que termino de enviar la señal modulada por callback
    if(a == 0)
    {
    	Modulador_sendStream(testStream2);
    }
    if(a == 1)
    {
    	Modulador_sendStream(dummyStream);
    }
    if(a == 2)
    {
    	Modulador_sendStream(testStream);
    }

    a++;
    if(a == 3)
    {
    	a = 0;
    }
*/
}

void testDecode(void)
{

	//Decoder_parsePulse(getPulseMeasure());

}

void testfunc(char c){
	char str[3];
	str[0] = c;
	str[1] = '\0';

	sendWord(str);
}

void testfunc2(char c){

	Modulador_sendChar(c);

}

/*******************************************************************************
 ******************************************************************************/
