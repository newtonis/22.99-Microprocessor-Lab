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
#include "SysTick.h"
#include "DisplayBoard.h"
#include "Encoder.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static int test_num[ID_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
static int test_pin[PIN_LEN] = {1, 2, 3, 4, 5};
static int rswitch_count_chk = 0;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

__ISR__ PORTA_IRQHandler (void){
	PORT_ClearInterruptFlag(RCHA);

}

__ISR__ PORTC_IRQHandler (void){
	PORT_ClearInterruptFlag(RCHB);

}

__ISR__ SysTick_Handler (void){
	DispShowPIN(test_pin);

	rswitch_count_chk++;
	if(!(rswitch_count_chk % 25)){
		if(!gpioRead(RSwitch)){
			gpioToggle(PIN_LED_BLUE);
			DispShiftMsj(PIN_MENU);
		}
	}

	if(!(rswitch_count_chk % 2)){
		if(EncGetFlag() == NONE){
			if(!gpioRead(RCHA)){
				if(test_pin[DispGetCursor()] == 0){
					test_pin[DispGetCursor()] = 9;
				}else{
					test_pin[DispGetCursor()]--;
				}
				EncSetFlag(LEFT);
			}else if(!(gpioRead(RCHB))){
				if(test_pin[DispGetCursor()] == 9){
					test_pin[DispGetCursor()] = 0;
				}else{
					test_pin[DispGetCursor()]++;
				}
				EncSetFlag(RIGHT);
			}
		}else{
			if(gpioRead(RCHA) && gpioRead(RCHB)){
				EncSetFlag(NONE);
			}
		}

	}
}

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    gpioMode(PIN_LED_BLUE, OUTPUT);

    gpioMode(RCHA, INPUT);
    gpioMode(RCHB, INPUT);
    gpioMode(RSwitch, INPUT);

    NVIC_EnableIRQ(SysTick_IRQn);
    SysTick_Init();

    DispBoard_Init();
    DispClear();
    EncClearFlag();
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
