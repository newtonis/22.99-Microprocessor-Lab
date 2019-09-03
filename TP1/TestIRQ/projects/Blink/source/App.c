/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "timer.h"
#include "DisplayBoard.h"
#include "Encoder.h"
#include "Lector.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
//static int test_num[ID_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
static int test_pin[PIN_LEN] = {1, 2, 3, 4, 5};
static tim_id_t timerDisp;
static tim_id_t timerEncCH;
static tim_id_t timerEncSwitch;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void displayHandler(void);

void encoderCH_Handler(void);

void enconderSwitch_Handler(void);

void modify_PIN(int pin_array[], int motion);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/*
__ISR__ PORTA_IRQHandler (void){
	PORT_ClearInterruptFlag(RCHA);

}

*/



void modify_PIN(int pin_array[], int motion){
	switch(motion){
	case RIGHT:
		if(pin_array[DispGetCursor()] == 9){
			pin_array[DispGetCursor()] = 0;
		}else{
			pin_array[DispGetCursor()]++;
		}
		break;
	case LEFT:
		if(pin_array[DispGetCursor()] == 0){
			pin_array[DispGetCursor()] = 9;
		}else{
			pin_array[DispGetCursor()]--;
		}
		break;
	}
}

void displayHandler (void){
	DispShowPIN(test_pin);

}

void encoderCH_Handler(void){

	encoderReadMotion(test_pin, modify_PIN);

}

void enconderSwitch_Handler(void){

	encoderReadSwitch(PIN_MENU, DispShiftMsj);

}

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    encoderInit();

    timerInit();

    timerDisp = timerGetId();
    timerStart(timerDisp, TIMER_MS2TICKS(20), TIM_MODE_PERIODIC, displayHandler);

    timerEncCH = timerGetId();
    timerStart(timerEncCH, TIMER_MS2TICKS(40), TIM_MODE_PERIODIC, encoderCH_Handler);

    timerEncSwitch = timerGetId();
    timerStart(timerEncSwitch, TIMER_MS2TICKS(500), TIM_MODE_PERIODIC, enconderSwitch_Handler);

    DispBoard_Init();
    DispClear();
    Lector_Init();

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	// Loop
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
