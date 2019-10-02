/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Sensores.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


static void i2c_handler (void);
static SRAWDATA mag;
static SRAWDATA accel;
static bool end = false;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

<<<<<<< HEAD:TP2/i2c/projects/Blink/source/App.c
void i2c_handler (void)
{
	end = true;
}
static read_data m;
=======
static void delayLoop(uint32_t veces);

static roll_t rolido = 0;
static pitching_t cabeceo = 0;
<<<<<<< HEAD:TP2/i2c/projects/Blink/source/App.c
<<<<<<< HEAD:TP2/i2c/projects/Blink/source/App.c
>>>>>>> parent of cf4fd69... edited accel:TP2/Marce/Blink/source/App.c
=======
>>>>>>> parent of cf4fd69... edited accel:TP2/Marce/Blink/source/App.c
=======
>>>>>>> parent of cf4fd69... edited accel:TP2/Marce/Blink/source/App.c

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{


}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
<<<<<<< HEAD:TP2/i2c/projects/Blink/source/App.c
=======
    rolido = Position_GetRoll();
    cabeceo = Position_GetPitch();
    delayLoop(4000000UL);
    gpioToggle(PIN_LED_BLUE);
}
>>>>>>> parent of cf4fd69... edited accel:TP2/Marce/Blink/source/App.c


	_mqx_ints_FXOS8700CQ_start();
	m.callback = i2c_handler;
	m.pAccelData = &accel;
	m.pMagnData = &mag;

	while(1)
	{
		ReadAccelMagnData(&m);
		while (end == false)
		{

		}
		end = false;
	}



}

