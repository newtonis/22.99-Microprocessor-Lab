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
#include "InternalControl.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
enum{ID_STAGE, PIN_STAGE, BRIGHT_EDIT, CHECKOUT, ERROR}; // FSM estados

#define ID_TEST			{I_CHAR, D_CHAR, ESPACIO, 1, 2, 3, 4, 5, 6, 7, 8}
#define PIN_TEST		{P_CHAR, I_CHAR, N_CHAR, 1, 2, 3, 4, 5}
#define CHECKOUT_TXT	{A_CHAR, C_CHAR, C_CHAR, E_CHAR, S_CHAR, O_CHAR}

static int id_vector[ID_WORD_LEN] = ID_TEST;
static int pin_vector[PIN_WORD_LEN] = PIN_TEST;
static int bright_vector[BRIGHT_LEN] = {L_CHAR, U_CHAR, 1, 0};
//static int checkout_vector[CHECKOUT_LEN] = CHECKOUT_TXT;

static user_t users[1] = {{.id = {4, 5, 1, 7, 6, 6, 0, 1}, .pin = {1, 1, 1, 1, 1}}};

static int default_id[ID_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
static int *aux_id;

static int fsm = ID_STAGE;

static tim_id_t timerDisp;
static tim_id_t timerSw;
static tim_id_t timerEncCH;
static tim_id_t timerEncSwitch;
static tim_id_t timerPestillo;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void displayHandler(void);

void encoderCH_Handler(void);

void enconderSwitch_Handler(void);

void readSwitchInterface(void);

void modifyNumberCode(int motion);

void closeDoor(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void modifyNumberCode(int motion){
	int *numCode;

	if(fsm == ID_STAGE){
		numCode = id_vector+3;
	}else if(PIN_STAGE){
		numCode = pin_vector+3;
	}


	if(DispModType() == NUM_TYPE){

		if(fsm == BRIGHT_EDIT){
			disp_bright_t aux_bright;
			aux_bright = DispChangeBright(motion);
			bright_vector[2] = aux_bright/10;
			bright_vector[3] = aux_bright%10;
		}else{
			switch(motion){
			case RIGHT:
				if(numCode[DispGetCursor()] == 9){
					numCode[DispGetCursor()] = 0;
				}else{
					numCode[DispGetCursor()]++;
				}
				break;
			case LEFT:
				if(numCode[DispGetCursor()] == 0){
					numCode[DispGetCursor()] = 9;
				}else{
					numCode[DispGetCursor()]--;
				}
				break;
			}
		}

	}else if(DispModType() == MENU_TYPE){
		switch(motion){
		case LEFT:
			if(fsm == ID_STAGE){
				fsm = BRIGHT_EDIT;
			}else{
				fsm--;
			}
			DispClear();
			break;
		case RIGHT:
			if(fsm == BRIGHT_EDIT){
				fsm = ID_STAGE;
			}else{
				fsm++;
			}
			DispClear();
			break;
		}
	}

}

void displayHandler (void){

	switch(fsm){
	case ID_STAGE:
		DispShowMsj(id_vector, ID_LEN, 2);
		break;
	case PIN_STAGE:
		DispShowMsj(pin_vector, PIN_LEN, 3);
		break;
	case BRIGHT_EDIT:
		DispShowMsj(bright_vector, BRIGHT_LEN, 2);
		break;
	case CHECKOUT:
		DispShowMsj(NULL, CHECKOUT_LEN, 0);
		break;
	}

}

void encoderCH_Handler(void){

	if(fsm <= BRIGHT_EDIT){
		modifyNumberCode(encoderReadMotion());
	}

}

void enconderSwitch_Handler(void){

	if(encoderReadSwitch()){
		if(fsm <= BRIGHT_EDIT){
			DispShiftMsj();
		}
	}

}

void readSwitchInterface(void){
	if(PortGetOK_Status()){
		int k = 0;

		switch(fsm){
		case ID_STAGE:
			while((users[0].id[k] == aux_id[k])&&(k < ID_LEN)){
				k++;
			}
			if(k == ID_LEN){
				fsm = PIN_STAGE;
				DispClear();
			}
			break;
		case PIN_STAGE:
			while((users[0].pin[k] == pin_vector[k+3])&&(k < PIN_LEN)){
				k++;
			}
			if(k == PIN_LEN){
				fsm = CHECKOUT;
				RGBIndicator(GREEN_INDICATOR);
				timerStart(timerPestillo, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, closeDoor);
				DispClear();
			}
			break;
		}

		PortClearOK_Status();
	}else if(PortGetCANCEL_Status()){
		DispClear();
		fsm = ID_STAGE;
		PortClearCANCEL_Status();
	}
}

void closeDoor(void){
	RGBIndicator(BLUE_INDICATOR);
	fsm = ID_STAGE;
	DispClear();
}

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	aux_id = default_id; // Asignacion de ID y PIN por default

	PortInit();

    encoderInit();

    timerInit();

    timerDisp = timerGetId();
    timerStart(timerDisp, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, displayHandler);

    timerEncCH = timerGetId();
    timerStart(timerEncCH, TIMER_MS2TICKS(40), TIM_MODE_PERIODIC, encoderCH_Handler);

    timerSw = timerGetId();
    timerStart(timerSw, TIMER_MS2TICKS(100), TIM_MODE_PERIODIC, readSwitchInterface);

    timerEncSwitch = timerGetId();
    timerStart(timerEncSwitch, TIMER_MS2TICKS(500), TIM_MODE_PERIODIC, enconderSwitch_Handler);

    timerPestillo = timerGetId(); // Lo disparo cuando se pueda abrir

    DispBoard_Init();
    DispClear();
    lectorInit();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

	switch(fsm){
	case ID_STAGE:
		//if(get_Enable()){
			aux_id = lector_get_PAN();
			//clear_Chk();
			for(int k = 0; k < ID_LEN; k++){
				id_vector[k+3] = aux_id[k];
			}
		//}
		break;
	case PIN_STAGE:
		// Se modifica solo con el encoder
		break;
	case BRIGHT_EDIT:
		break;
	case CHECKOUT:
		break;
	case ERROR:
		break;
	}

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
