/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "timer.h"
#include "Posicionamiento.h"
#include "CAN.h"
#include "SPI.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/// Idea del programa final ///
enum{ROLL_REFRESH, PITCH_REFRESH, ORIENT_REFRESH};
uint8_t fsm = ROLL_REFRESH;

tim_id_t timPeriodico;

roll_t roll_app;
pitching_t pitching_app;
orientation_t orientation_app;
///////////////////////////////

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void periodicRefresh(void);

void test_irq(void)
{

}

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	SPI_driver_init();
	init_CAN(0x102, test_irq);
    char buf[5] = {0x82, 0x43, 0x01, 0x02, 0x03};
    send_CAN(0x102, buf, 5);
/*
    char buf2[3] = {0x85, 0x23, 0xFA};
    send_CAN(0x102, buf2, 3);

    /// Idea del programa final ///
    timPeriodico = timerGetId();

    timerStart(timPeriodico, TIMER_MS2TICKS(1000), TIM_MODE_PERIODIC, periodicRefresh);

    ///////////////////////////////

     */
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
 ******************************************************************************/
