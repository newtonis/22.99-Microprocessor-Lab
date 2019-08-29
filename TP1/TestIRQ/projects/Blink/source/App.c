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

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static int segments[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
static int disp_selector[] = {SEL0, SEL1};
static int status_leds[] = {STAT0, STAT1};
static int disp_num = 0;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void Disp7Seg_Init(void);

void Disp7Seg_Write(int num);

bool Disp_Select(int disp);

void Status_Write(int code);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
__ISR__ PORTA_IRQHandler (void){
	PORT_ClearInterruptFlag(PIN_SW3);
	gpioToggle(PIN_LED_BLUE);
}

__ISR__ SysTick_Handler (void){
	gpioToggle(PIN_LED_BLUE);
	Disp_Select(disp_num);
	Disp7Seg_Write(7);
	disp_num++;
	if(disp_num > 3){
		disp_num = 0;
	}

}

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    gpioMode(PIN_LED_BLUE, OUTPUT);
    gpioMode(PIN_SW3, INPUT);
    gpioMode(PIN_SW2, INPUT_PULLUP);

    gpioIRQC(PIN_SW3, INTERRUPT_RISING_EDGE);
    NVIC_EnableIRQ(PORTA_IRQn);

    NVIC_EnableIRQ(SysTick_IRQn);
    SysTick_Init();

    Disp7Seg_Init();
    Disp_Select(0);
    Status_Write(1);
    Status_Write(2);
    Status_Write(3);
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

void Disp7Seg_Init(void){
	int i = 0;
	for(i = 0; i < 2; i++){
		gpioMode(disp_selector[i], OUTPUT);
	}
	for(i = 0; i < 8; i++){
		gpioMode(segments[i], OUTPUT);
	}
	for(i = 0; i < 2; i++){
		gpioMode(status_leds[i], OUTPUT);
	}
}

void Disp7Seg_Write(int num){
	switch(num){
	case 0:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], LOW);
		gpioWrite(segments[7], LOW);
		break;
	case 1:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], LOW);
		gpioWrite(segments[4], LOW);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], LOW);
		gpioWrite(segments[7], LOW);
		break;
	case 2:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], LOW);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case 3:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], LOW);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case 4:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], LOW);
		gpioWrite(segments[4], LOW);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case 5:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], LOW);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case 6:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case 7:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], LOW);
		gpioWrite(segments[4], LOW);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], LOW);
		gpioWrite(segments[7], LOW);
		break;
	case 8:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case 9:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], LOW);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	}
}

bool Disp_Select(int disp){
	switch(disp){
	case 0:
		gpioWrite(disp_selector[0], LOW);
		gpioWrite(disp_selector[1], LOW);
		return 1;
	case 1:
		gpioWrite(disp_selector[0], HIGH);
		gpioWrite(disp_selector[1], LOW);
		return 1;
	case 2:
		gpioWrite(disp_selector[0], LOW);
		gpioWrite(disp_selector[1], HIGH);
		return 1;
	case 3:
		gpioWrite(disp_selector[0], HIGH);
		gpioWrite(disp_selector[1], HIGH);
		return 1;
	default:
		return 0;
	}
}

void Status_Write(int code){
	switch(code){
	case 1:
		gpioWrite(status_leds[0], HIGH);
		gpioWrite(status_leds[1], LOW);
		break;
	case 2:
		gpioWrite(status_leds[0], LOW);
		gpioWrite(status_leds[1], HIGH);
		break;
	case 3:
		gpioWrite(status_leds[0], HIGH);
		gpioWrite(status_leds[1], HIGH);
		break;
	default:
		break;
	}
}

/*******************************************************************************
 ******************************************************************************/
