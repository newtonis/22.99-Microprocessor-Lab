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
#include "Users.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
enum{ID_STAGE, BRIGHT_EDIT, PIN_STAGE, CHECKOUT_STAGE, ERROR_STAGE, ID_ERROR_STAGE}; // FSM estados
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


static int fsm = ID_STAGE;

static tim_id_t timerError;
static tim_id_t timerPestillo;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void internarHandler(void);

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



/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	internalControlInit(internarHandler);
/*
	initUser();

    timerInit();

    timerDisp = timerGetId();
    timerStart(timerDisp, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, displayHandler);

    timerIdle = timerGetId();
    timerStart(timerIdle, TIMER_MS2TICKS(3000), TIM_MODE_PERIODIC, idleHandler);

    timerPestillo = timerGetId(); // Lo disparo cuando se pueda abrir

    timerError = timerGetId(); // Lo dispador cuando no es valido el usuario


    */
	RGBIndicator(BLUE_INDICATOR);
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



/*******************************************************************************
 ******************************************************************************/
