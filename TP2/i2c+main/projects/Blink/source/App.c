/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Posicionamiento.h"
#include "timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

enum{ROLL_REFRESH, PITCH_REFRESH, ORIENT_REFRESH};
uint8_t fsm = ROLL_REFRESH;

tim_id_t timPeriodico;
tim_id_t timerUpdatePos;

roll_t roll_app;
pitching_t pitching_app;
orientation_t orientation_app;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void periodicRefresh(void);
void test (void);

void test(void)
{

}

void periodicRefresh(void)
{
	// Cada 1 segundo refresca uno de los parámetros
	switch (fsm) {
		case ROLL_REFRESH:
			roll_app = Position_GetRoll();
			// ENVIAR A PC NUEVO DATO POR UART
			break;
		case PITCH_REFRESH:
			pitching_app = Position_GetPitch();
			// Enviar a PC NUEVO DATO POR UART
			break;
		case ORIENT_REFRESH:
			orientation_app = Position_GetOrientation();
			// ENVIAR A PC NUEVO DATO POR UART
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

