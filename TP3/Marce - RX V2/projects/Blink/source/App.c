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
static bool testStream[STAND_LEN] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1};
static bool dummyStream[STAND_LEN] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static uint8_t a = 0;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void test(void);

void testDecode(void);

static void delayLoop(uint32_t veces);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    gpioMode(PIN_LED_BLUE, OUTPUT);
	ftmInit(testDecode);
    CMP_init(0);
    Modulador_init(getDutyAddress(), test);

    a = 1;

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
    Modulador_sendStream(dummyStream);
    delayLoop(4000000UL);
    Modulador_sendStream(testStream);
    delayLoop(4000000UL);
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void test(void)
{
	a; // avisa que termino de enviar la señal modulada por callback
}

void testDecode(void)
{
	if(a == 1)
	{
		Decoder_parsePulse(getPulseMeasure());
	}

}

static void delayLoop(uint32_t veces)
{
    while (veces--);
}

/*******************************************************************************
 ******************************************************************************/
