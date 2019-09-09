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
#include "Users.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
enum{ID_STAGE, PIN_STAGE, BRIGHT_EDIT, CHECKOUT_STAGE, ERROR_STAGE}; // FSM estados

#define ID_LEN			8
#define PIN_LEN			5
#define BRIGHT_LEN		1

#define ID_WORD_LEN		11
#define PIN_WORD_LEN	8
#define BRIGHT_WORD_LEN 4

#define PIN_MENU		1
#define ID_MENU			2
#define BRIGHT_MENU		3

#define ID_TEST			{I_CHAR, D_CHAR, ESPACIO, GUION, GUION, GUION, GUION, GUION, GUION, GUION, GUION}
#define PIN_TEST		{P_CHAR, I_CHAR, N_CHAR, 1, 2, 3, 4, 5}
#define HIDE_PIN_TEST	{P_CHAR, I_CHAR, N_CHAR, GUION, GUION, GUION, GUION, GUION}
#define CHECKOUT_OK_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, A_CHAR, C_CHAR, C_CHAR, E_CHAR, S_CHAR, O_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}
#define CHECKOUT_ERR_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, C_CHAR, O_CHAR, D_CHAR, E_CHAR, ESPACIO, E_CHAR, R_CHAR, R_CHAR, O_CHAR, R_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}

static disp_msj_t id_txt = {ID_TEST, ID_LEN, 2, 1};

static disp_msj_t pin_txt = {PIN_TEST, PIN_LEN, 3, 1};
static disp_msj_t pin_toDisp = {HIDE_PIN_TEST, PIN_LEN, 3, 1}; // Para enviar este con los caracteres ocultos

#define BRIGHT_UNIT		3
#define BRIGHT_DEC		2
static disp_msj_t bright_txt = {{L_CHAR, U_CHAR, 1, 0}, BRIGHT_LEN, 2, 2};

static int checkoutOK_vec[] = CHECKOUT_OK_TXT;
static disp_msj_t checkoutOK_txt = {CHECKOUT_OK_TXT, SIZEOFARR(checkoutOK_vec), 0, 0};

static int checkoutERROR_vec[] = CHECKOUT_ERR_TXT;
static disp_msj_t checkoutERROR_txt = {CHECKOUT_ERR_TXT, SIZEOFARR(checkoutERROR_vec), 0, 0};

static int default_id[ID_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
static int *aux_id;

static int fsm = ID_STAGE;

static tim_id_t timerDisp;
static tim_id_t timerError;
static tim_id_t timerPestillo;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void displayHandler(void);

void encoderHandler(void);

void internarHandler(void);

void modifyNumberCode(int motion);

void update_hidePIN(void);

void clearUserInfo(void);

void closeDoor(void);

void accessDenied(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void clearUserInfo(void){
	int aux[] = ID_TEST;
	for(int k = 0; k < ID_WORD_LEN; k++){
		id_txt.array[k] = aux[k];
	}
	int aux2[] = PIN_TEST;
	for(int k = 0; k < PIN_WORD_LEN; k++){
		pin_txt.array[k] = aux2[k];
	}
}

void update_hidePIN(void){
	int aux[] = HIDE_PIN_TEST;
	for(int k = 0; k < PIN_WORD_LEN; k++){
		pin_toDisp.array[k] = aux[k];
	}

	if(DispModType() == NUM_TYPE){
		pin_toDisp.array[DispGetCursor()+3] = pin_txt.array[DispGetCursor()+3];
	}
}

void modifyNumberCode(int motion){
	int *numCode;

	if(fsm == ID_STAGE){
		numCode = (id_txt.array)+3;
	}else if(PIN_STAGE){
		numCode = (pin_txt.array)+3;
	}


	if(DispModType() == NUM_TYPE){

		if(fsm == BRIGHT_EDIT){
			disp_bright_t aux_bright;
			aux_bright = DispChangeBright(motion);
			bright_txt.array[BRIGHT_DEC] = aux_bright/10;
			bright_txt.array[BRIGHT_UNIT] = aux_bright%10;
		}else{
			switch(motion){
			case RIGHT:

				if(numCode[DispGetCursor()] == GUION){ // Aplica mas que nada para el ID reseteado
					numCode[DispGetCursor()] = 0;
				}else{
					if(numCode[DispGetCursor()] == 9){
						numCode[DispGetCursor()] = 0;
					}else{
						numCode[DispGetCursor()]++;
					}
				}

				break;
			case LEFT:

				if(numCode[DispGetCursor()] == GUION){
					numCode[DispGetCursor()] = 9;
				}else{
					if(numCode[DispGetCursor()] == 0){
						numCode[DispGetCursor()] = 9;
					}else{
						numCode[DispGetCursor()]--;
					}
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
		DispShowMsj(id_txt);
		break;
	case PIN_STAGE:
		update_hidePIN();
		DispShowMsj(pin_toDisp);
		break;
	case BRIGHT_EDIT:
		DispShowMsj(bright_txt);
		break;
	case CHECKOUT_STAGE:
		DispShowMsj(checkoutOK_txt);
		break;
	case ERROR_STAGE:
		DispShowMsj(checkoutERROR_txt);
		break;
	}

}

void encoderHandler(void){
	if(fsm <= BRIGHT_EDIT){
		int event = encoderMotionGetEvent();
		switch(event){
		case LEFT:
			modifyNumberCode(event);
			break;
		case RIGHT:
			modifyNumberCode(event);
			break;
		case ENTER:
			DispShiftMsj();
			break;
		}
	}
}


void lectorHandler(void){
	if(fsm == ID_STAGE){
		//if(get_Enable()){ //probar con if(newReadAvailable())
		aux_id = lector_get_PAN();
		//clear_Chk(); //sin esta linea
		for(int k = 0; k < ID_LEN; k++){ // Me quedo solo con la parte que me interesa del PAN
			id_txt.array[k+3] = aux_id[k];
		}
	}
}

void internarHandler(void){
	switch(internalControlGetEvent()){
	case OK_EVENT:
		if(validateUser((id_txt.array)+3, (pin_txt.array)+3)){
			fsm = CHECKOUT_STAGE;
			DispClear();
			timerStart(timerPestillo, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, closeDoor);
			RGBIndicator(GREEN_INDICATOR);
		}else{
			fsm = ERROR_STAGE;
			DispClear();
			timerStart(timerError, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, accessDenied);
			RGBIndicator(RED_INDICATOR);
		}
		break;
	case CANCEL_EVENT:
		fsm = ID_STAGE;
		clearUserInfo();
		break;
	}
}

void closeDoor(void){
	RGBIndicator(BLUE_INDICATOR);
	fsm = ID_STAGE;
	DispClear();
	clearUserInfo();
}

void accessDenied(void){
	RGBIndicator(BLUE_INDICATOR);
	fsm = ID_STAGE;
	DispClear();
	clearUserInfo();
}

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	aux_id = default_id; // Asignacion de ID por default

	initUser();

    timerInit();

	internalControlInit(internarHandler);

    encoderInit(encoderHandler);

    timerDisp = timerGetId();
    timerStart(timerDisp, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, displayHandler);

    timerPestillo = timerGetId(); // Lo disparo cuando se pueda abrir

    timerError = timerGetId(); // Lo dispador cuando no es valido el usuario

    DispBoard_Init();

    DispClear();

    lectorInit(lectorHandler);

    RGBIndicator(BLUE_INDICATOR);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	switch(fsm){
	case ID_STAGE:
		break;
	case PIN_STAGE:
		// Se modifica solo con el encoder

		break;
	case BRIGHT_EDIT:
		// Tambien se modifica solo con el encoder

		break;
	case CHECKOUT_STAGE:
		break;
	case ERROR_STAGE:
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
