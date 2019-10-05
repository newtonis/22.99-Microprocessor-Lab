/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "Posicionamiento.h"
#include "ExternalManager.h"
#include "timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

enum{ROLL_REFRESH, PITCH_REFRESH, ORIENT_REFRESH};
uint8_t fsm = ROLL_REFRESH;

tim_id_t timPeriodico;
tim_id_t timerUpdatePos;

BoardParams_t AppBoard;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void periodicRefresh(void);
void test (void);

/*******************************************************************************
 * PRIVATE FUNCTIONS DEFINITIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void test(void)
{
	switch (Position_GetChangeEvent()) {
		case ROLL_EVENT:
			AppBoard.roll = Position_GetRoll();
			ExternManager_send2Ext(AppBoard, ROLL_UPD);
			break;
		case PITCHING_EVENT:
			AppBoard.pitching = Position_GetPitch();
			ExternManager_send2Ext(AppBoard, PITCHING_UPD);
			break;
		case ORIENTATION_EVENT:
			AppBoard.orientation = Position_GetOrientation();
			ExternManager_send2Ext(AppBoard, ORIENTATION_UPD);
			break;
	}
}

void periodicRefresh(void)
{
	int a;
	a = 1;
	// Cada 1 segundo refresca uno de los parámetros
	switch (fsm) {
		case ROLL_REFRESH:
			AppBoard.roll = Position_GetRoll();
			ExternManager_send2Ext(AppBoard, ROLL_UPD);
			break;
		case PITCH_REFRESH:
			AppBoard.pitching = Position_GetPitch();
			ExternManager_send2Ext(AppBoard, PITCHING_UPD);
			break;
		case ORIENT_REFRESH:
			AppBoard.orientation = Position_GetOrientation();
			ExternManager_send2Ext(AppBoard, ORIENTATION_UPD);
			break;
	}


	if(fsm == ORIENT_REFRESH)
	{
		fsm = ROLL_REFRESH;
	}
	else
	{
		fsm++;
	}
}



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	//ExternManager_init(2);

	Position_InitDrv(test);

	timerInit();

	timerUpdatePos = timerGetId();
	timerStart(timerUpdatePos, TIMER_MS2TICKS(200), TIM_MODE_PERIODIC, Position_Update);

	timPeriodico = timerGetId();
	timerStart(timPeriodico, TIMER_MS2TICKS(1000), TIM_MODE_PERIODIC, periodicRefresh);

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

}

