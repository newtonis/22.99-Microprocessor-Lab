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


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static bool testStream[STAND_LEN] =  {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1};
static bool testStream2[STAND_LEN] = {0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0};
static bool dummyStream[STAND_LEN] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static uint8_t a = 0;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void test(void);

void testDecode(void);

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
    gpioMode(PIN_LED_BLUE, OUTPUT);
	ftmInit(incrementOvfCnt);
    CMP_init(0);
    Modulador_init(getDutyAddress(), test);
    Decoder_init(getMedAddress());

    Modulador_sendStream(dummyStream);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void test(void)
{
	a++; // avisa que termino de enviar la señal modulada por callback
    if(a % 2)
    {
    	Modulador_sendStream(testStream);
    }
    else
    {
    	Modulador_sendStream(testStream2);
    }

}

void testDecode(void)
{

	//Decoder_parsePulse(getPulseMeasure());

}

/*******************************************************************************
 ******************************************************************************/
