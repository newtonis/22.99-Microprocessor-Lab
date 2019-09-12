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
enum{ID_STAGE, BRIGHT_EDIT, PIN_STAGE, CHECKOUT_STAGE, ERROR_STAGE, ID_ERROR_STAGE, USER_MENU}; // FSM estados

enum{MAIN_MENU, OPT_SEL, PIN_CHANGER};
enum{OPEN_DOOR = 1, PIN_MODIFY};
enum{NEW_PIN_MSJ, NEW_PIN_MENU, CONF_PIN_MSJ, CONF_PIN_MENU, PIN_OK_CHANGED, PIN_NO_CHANGED};
static int user_fsm = MAIN_MENU;
static int pin_change_fsm = NEW_PIN_MSJ;


enum{NOT_IDLE, IDLE};

#define ID_LEN			8
#define PIN_LEN			5
#define BRIGHT_LEN		1
#define SEL_LEN			1

#define OPTIONS			2

#define ID_WORD_LEN		11
#define PIN_WORD_LEN	8

#define PIN_MENU		1
#define ID_MENU			2
#define BRIGHT_MENU		3

#define ID_TEST			{I_CHAR, D_CHAR, ESPACIO, GUION, GUION, GUION, GUION, GUION, GUION, GUION, GUION}
#define PIN_TEST		{P_CHAR, I_CHAR, N_CHAR, 1, 2, 3, 4, 5}
#define HIDE_PIN_TEST	{P_CHAR, I_CHAR, N_CHAR, GUION, GUION, GUION, GUION, GUION}
#define CHECKOUT_OK_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, A_CHAR, C_CHAR, C_CHAR, E_CHAR, S_CHAR, O_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}
#define CHECKOUT_ERR_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, C_CHAR, O_CHAR, D_CHAR, E_CHAR, ESPACIO, E_CHAR, R_CHAR, R_CHAR, O_CHAR, R_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}
#define ID_ERROR_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, N_CHAR, O_CHAR, ESPACIO, S_CHAR, U_CHAR, C_CHAR, H_CHAR, ESPACIO, I_CHAR, D_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}

#define HELLO_TXT		{GUION, GUION, U_CHAR, S_CHAR, E_CHAR, R_CHAR, ESPACIO, O_CHAR, P_CHAR, S_CHAR, GUION, GUION, 1, ESPACIO, O_CHAR, P_CHAR, E_CHAR, N_CHAR, GUION, 2, ESPACIO, N_CHAR, E_CHAR, U_CHAR, ESPACIO, P_CHAR, I_CHAR, N_CHAR, GUION, GUION}
#define SEL_TXT			{S_CHAR, E_CHAR, L_CHAR, 1}
#define NEW_PIN_TXT		{ESPACIO, ESPACIO, ESPACIO, ESPACIO, C_CHAR, H_CHAR, O_CHAR, O_CHAR, S_CHAR, E_CHAR, ESPACIO, N_CHAR, E_CHAR, U_CHAR, ESPACIO, P_CHAR, I_CHAR, N_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}
#define CONF_PIN_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, C_CHAR, O_CHAR, N_CHAR, F_CHAR, I_CHAR, R_CHAR, N_CHAR, ESPACIO, N_CHAR, E_CHAR, U_CHAR, ESPACIO, P_CHAR, I_CHAR, N_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}
#define MODF_OK_TXT		{ESPACIO, ESPACIO, ESPACIO, ESPACIO, P_CHAR, I_CHAR, N_CHAR, ESPACIO, C_CHAR, H_CHAR, A_CHAR, N_CHAR, G_CHAR, E_CHAR, D_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}

static disp_msj_t id_txt = {ID_TEST, ID_LEN, 2, 1};

static disp_msj_t pin_txt = {PIN_TEST, PIN_LEN, 3, 1};
static disp_msj_t pin_toDisp = {HIDE_PIN_TEST, PIN_LEN, 3, 1}; // Para enviar este con los caracteres ocultos
static int aux_pin[PIN_LEN] = {0, 0, 0, 0, 0};

#define BRIGHT_UNIT		3
#define BRIGHT_DEC		2
static disp_msj_t bright_txt = {{L_CHAR, U_CHAR, 1, 0}, BRIGHT_LEN, 2, 2};

static int checkoutOK_vec[] = CHECKOUT_OK_TXT;
static disp_msj_t checkoutOK_txt = {CHECKOUT_OK_TXT, SIZEOFARR(checkoutOK_vec), 0, 0};

static int checkoutERROR_vec[] = CHECKOUT_ERR_TXT;
static disp_msj_t checkoutERROR_txt = {CHECKOUT_ERR_TXT, SIZEOFARR(checkoutERROR_vec), 0, 0};

static int idError_vec[] = ID_ERROR_TXT;
static disp_msj_t idError_txt = {ID_ERROR_TXT, SIZEOFARR(idError_vec), 0, 0};

static int hello_vec[] = HELLO_TXT;
static disp_msj_t hello_txt = {HELLO_TXT, SIZEOFARR(hello_vec), 0, 0};

static disp_msj_t sel_txt = {SEL_TXT, SEL_LEN, 3, 1};

static int new_pin_vec[] = NEW_PIN_TXT;
static disp_msj_t new_pin_txt = {NEW_PIN_TXT, SIZEOFARR(new_pin_vec), 0, 0};

static int conf_pin_vec[] = CONF_PIN_TXT;
static disp_msj_t conf_pin_txt = {CONF_PIN_TXT, SIZEOFARR(conf_pin_vec), 0, 0};

static int modf_ok_vec[] = MODF_OK_TXT;
static disp_msj_t modf_ok_txt = {MODF_OK_TXT, SIZEOFARR(modf_ok_vec), 0, 0};

static int default_id[ID_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
static int *aux_id;


static int idle_cnt = 0;
static int fsm = ID_STAGE;
static bool idle_fsm = 0;

static tim_id_t timerDisp;
static tim_id_t timerIdle;
static tim_id_t timerError;
static tim_id_t timerPestillo;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void displayHandler(void);

void encoderHandler(void);

void idleHandler(void);

void internarHandler(void);

void modifyNumberCode(int motion);

void update_hidePIN(void);

void clearUserInfo(void);

void clearPINInfo(void);

void pinFsmUpdate(void);

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

void clearPINInfo(void){
	int aux2[] = PIN_TEST;
	for(int k = 0; k < PIN_WORD_LEN; k++){
		pin_txt.array[k] = aux2[k];
	}
}

void pinFsmUpdate(void){
	if(pin_change_fsm == PIN_NO_CHANGED){
		pin_change_fsm = NEW_PIN_MSJ;
		user_fsm = MAIN_MENU;
		clearUserInfo();
		fsm = ID_STAGE;
		DispClear();
	}else if(pin_change_fsm == PIN_OK_CHANGED){
		pin_change_fsm = NEW_PIN_MSJ;
		user_fsm = MAIN_MENU;
		DispClear();
	}else{
		pin_change_fsm++;
		DispClear();
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
	}else if((fsm == PIN_STAGE)||(pin_change_fsm == NEW_PIN_MENU)||(pin_change_fsm == CONF_PIN_MENU)){
		numCode = (pin_txt.array)+3;
	}else if(fsm == USER_MENU){
		if(user_fsm == OPT_SEL){
			numCode = (sel_txt.array)+3;
		}
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

				if(user_fsm == OPT_SEL){
					if(numCode[DispGetCursor()] > OPTIONS){
						numCode[DispGetCursor()] = 1;
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

				if(user_fsm == OPT_SEL){
					if(numCode[DispGetCursor()] < 1){
						numCode[DispGetCursor()] = OPTIONS;
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
			}else if(fsm == BRIGHT_EDIT){
				fsm = ID_STAGE;

			}else if(fsm == USER_MENU){
				if(user_fsm == MAIN_MENU){
					user_fsm = OPT_SEL;
				}else if(user_fsm == OPT_SEL){
					user_fsm = MAIN_MENU;
				}
			}
			DispClear();
			break;
		case RIGHT:
			if(fsm == ID_STAGE){
				fsm = BRIGHT_EDIT;
			}else if(fsm == BRIGHT_EDIT){
				fsm = ID_STAGE;

			}else if(fsm == USER_MENU){
				if(user_fsm == MAIN_MENU){
					user_fsm = OPT_SEL;
				}else if(user_fsm == OPT_SEL){
					user_fsm = MAIN_MENU;
				}
			}
			DispClear();
			break;
		}
	}

}

void idleHandler(void){
	switch(idle_fsm){
	case IDLE:
		clearUserInfo();
		fsm = ID_STAGE;
		idle_fsm = !idle_fsm;
		break;
	case NOT_IDLE:
		idle_cnt++;
		if(idle_cnt == 30){
			idle_fsm = IDLE;
		}
		break;
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
	case ID_ERROR_STAGE:
		DispShowMsj(idError_txt);
		break;
	case USER_MENU:

		switch(user_fsm){
		case MAIN_MENU:
			DispShowMsj(hello_txt);
			break;
		case OPT_SEL:
			DispShowMsj(sel_txt);
			break;
		case PIN_CHANGER:

			switch(pin_change_fsm){
			case NEW_PIN_MSJ:
				DispShowMsj(new_pin_txt);
				break;
			case NEW_PIN_MENU:
				update_hidePIN();
				DispShowMsj(pin_toDisp);
				break;
			case CONF_PIN_MSJ:
				DispShowMsj(conf_pin_txt);
				break;
			case CONF_PIN_MENU:
				update_hidePIN();
				DispShowMsj(pin_toDisp);
				break;
			case PIN_OK_CHANGED:
				DispShowMsj(modf_ok_txt);
				break;
			case PIN_NO_CHANGED:
				DispShowMsj(checkoutERROR_txt);
				break;
			}

			break;
		}

		break;
	}

}

void encoderHandler(void){
	if((fsm <= PIN_STAGE)||(fsm == USER_MENU)){
		idle_cnt = 0;
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
		idle_cnt = 0;
		aux_id = lector_get_PAN();
		for(int k = 0; k < ID_LEN; k++){ // Me quedo solo con la parte que me interesa del PAN
			id_txt.array[k+3] = aux_id[k];
		}
	}
}

void internarHandler(void){
	bool pin_match = 1;

	switch(internalControlGetEvent()){
	case OK_EVENT:

		if(fsm == ID_STAGE){

			if(validateID((id_txt.array)+3)){
				fsm = PIN_STAGE;
				DispClear();
			}else{
				fsm = ID_ERROR_STAGE;
				timerStart(timerError, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, accessDenied);
				RGBIndicator(RED_INDICATOR);
				DispClear();
			}

		}else if(fsm == PIN_STAGE){
			if(validateUser((id_txt.array)+3, (pin_txt.array)+3)){
				fsm = USER_MENU;
				DispClear();
			}else{
				fsm = ERROR_STAGE;
				DispClear();
				timerStart(timerError, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, accessDenied);
				RGBIndicator(RED_INDICATOR);
			}
		}else if(fsm == USER_MENU){
			if(user_fsm == OPT_SEL){
				switch(sel_txt.array[3]){
				case OPEN_DOOR:
					fsm = CHECKOUT_STAGE;
					user_fsm = MAIN_MENU;
					DispClear();
					timerStart(timerPestillo, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, closeDoor);
					RGBIndicator(GREEN_INDICATOR);
					break;
				case PIN_MODIFY:
					user_fsm = PIN_CHANGER;
					pin_change_fsm = NEW_PIN_MSJ;
					timerStart(timerPestillo, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, pinFsmUpdate);
					DispClear();
					break;
				}
			}else if(user_fsm == PIN_CHANGER){
				switch(pin_change_fsm){
				case NEW_PIN_MENU:
					for(int k = 0; k < PIN_LEN; k++){
						aux_pin[k] = pin_txt.array[k+3]; // Autosave
					}
					clearPINInfo();
					pinFsmUpdate();
					timerStart(timerPestillo, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, pinFsmUpdate);
					DispClear();
					break;
				case CONF_PIN_MENU:

					for(int k = 0; k < PIN_LEN; k++){
						if(pin_txt.array[k+3] != aux_pin[k]){
							pin_match = 0;
						}
					}
					if(pin_match){
						changePin((id_txt.array)+3, aux_pin);
						pinFsmUpdate();
						timerStart(timerPestillo, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, pinFsmUpdate);
						DispClear();
					}else{
						pin_change_fsm = PIN_NO_CHANGED;
						timerStart(timerPestillo, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, pinFsmUpdate);
						DispClear();
					}
					break;
				}
			}
		}

		break;
	case CANCEL_EVENT:
		fsm = ID_STAGE;
		clearUserInfo();
		DispClear();
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

    timerIdle = timerGetId();
    timerStart(timerIdle, TIMER_MS2TICKS(3000), TIM_MODE_PERIODIC, idleHandler);

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
		if(LedStatus_GetState() != 1){
			LedStatus_Write(1);
		}
		break;
	case PIN_STAGE:
		// Se modifica solo con el encoder
		if(LedStatus_GetState() != 2){
			LedStatus_Write(2);
		}
		break;
	case BRIGHT_EDIT:
		// Tambien se modifica solo con el encoder
		break;
	case CHECKOUT_STAGE:
		if(LedStatus_GetState() != 3){
			LedStatus_Write(3);
		}
		break;
	case ERROR_STAGE:
		break;
	case ID_ERROR_STAGE:
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
