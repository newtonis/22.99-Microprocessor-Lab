/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "timer.h"
#include "InternalControl.h"
#include "SPI.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
enum{ID_STAGE, BRIGHT_EDIT, PIN_STAGE, CHECKOUT_STAGE, ERROR_STAGE, ID_ERROR_STAGE,MESSAGE_RECEIVED}; // FSM estados
enum{NOT_IDLE, IDLE}; // IDLE estados

#define ID_LEN			8
#define PIN_LEN			5
#define BRIGHT_LEN		1
#define TEXT_TAB		3

#define ID_WORD_LEN		11
#define PIN_WORD_LEN	8

#define PIN_MENU		1
#define ID_MENU			2



#define BRIGHT_UNIT		3
#define BRIGHT_DEC		2

#define MAX_MESSAGES	3

static int fsm = ID_STAGE;


static uint16_t messages[MAX_MESSAGES]= {0,0,0};

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void internarHandler(void);

void spiFallingEdgeOnINTPinHandler(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



void internarHandler(void){
	switch(internalControlGetEvent()){
	case OK_EVENT:

		if(fsm == ID_STAGE){

		}else if(fsm == PIN_STAGE){
		}

		break;
	case CANCEL_EVENT:
		fsm = ID_STAGE;
		break;
	}
}

void spiFallingEdgeOnINTPinHandler(void){ // este callback es especifico de este evento
	uint8_t SPI_n = 0;
	fsm = MESSAGE_RECEIVED;
}




/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	SPI_Initialize(spiFallingEdgeOnINTPinHandler);
	//SPI_driver_init();
	internalControlInit(internarHandler);
	RGBIndicator(BLUE_INDICATOR);

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

	testSPI(SPI_0);
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
