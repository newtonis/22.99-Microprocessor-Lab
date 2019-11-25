/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "hardware.h"
#include  <os.h>
#include "DisplayBoard.h"
#include "Encoder.h"
#include "Lector.h"
#include "InternalControl.h"
#include "Users.h"
#include "PIT.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

enum{ID_STAGE, BRIGHT_EDIT, PIN_STAGE, CHECKOUT_STAGE, ERROR_STAGE, ID_ERROR_STAGE}; // FSM estados
enum{NOT_IDLE, IDLE}; // IDLE estados

#define ID_LEN			8
#define PIN_LEN			5
#define BRIGHT_LEN		1
#define TEXT_TAB		3

#define ID_WORD_LEN		11
#define PIN_WORD_LEN	8

#define PIN_MENU		1
#define ID_MENU			2

#define ID_TEST			{I_CHAR, D_CHAR, ESPACIO, GUION, GUION, GUION, GUION, GUION, GUION, GUION, GUION}
#define PIN_TEST		{P_CHAR, I_CHAR, N_CHAR, 1, 2, 3, 4, 5}
#define HIDE_PIN_TEST	{P_CHAR, I_CHAR, N_CHAR, GUION, GUION, GUION, GUION, GUION}
#define CHECKOUT_OK_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, A_CHAR, C_CHAR, C_CHAR, E_CHAR, S_CHAR, O_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}
#define CHECKOUT_ERR_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, C_CHAR, O_CHAR, D_CHAR, E_CHAR, ESPACIO, E_CHAR, R_CHAR, R_CHAR, O_CHAR, R_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}
#define ID_ERROR_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, N_CHAR, O_CHAR, ESPACIO, S_CHAR, U_CHAR, C_CHAR, H_CHAR, ESPACIO, I_CHAR, D_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}

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

static int idError_vec[] = ID_ERROR_TXT;
static disp_msj_t idError_txt = {ID_ERROR_TXT, SIZEOFARR(idError_vec), 0, 0};

static int default_id[ID_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
static int *aux_id;

static int idle_cnt = 0;
static int fsm = ID_STAGE;
static bool idle_fsm = 0;

static bool t_switch; // si es false es access denied si es true cierra la puerta


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

void closeDoor(void);

void accessDenied(void);

void timerSwitch (void);


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


void timerSwitch (void)
{
	if(t_switch == true)
	{
		closeDoor();
	}
	else{
		accessDenied();
	}
}

void update_hidePIN(void){
	int aux[] = HIDE_PIN_TEST;
	for(int k = 0; k < PIN_WORD_LEN; k++){
		pin_toDisp.array[k] = aux[k];
	}

	if(DispModType() == NUM_TYPE){
		pin_toDisp.array[DispGetCursor()+TEXT_TAB] = pin_txt.array[DispGetCursor()+TEXT_TAB];
	}
}

void modifyNumberCode(int motion){
	int *numCode;

	if(fsm == ID_STAGE){
		numCode = (id_txt.array)+TEXT_TAB;
	}else if(PIN_STAGE){
		numCode = (pin_txt.array)+TEXT_TAB;
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
			}else if(fsm != PIN_STAGE){
				fsm--;
			}
			DispClear();
			break;
		case RIGHT:
			if(fsm == BRIGHT_EDIT){
				fsm = ID_STAGE;
			}else if(fsm != PIN_STAGE){
				fsm++;
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
	}

}

void encoderHandler(void){
	if(fsm <= PIN_STAGE){
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
			id_txt.array[k+TEXT_TAB] = aux_id[k];
		}
	}
}

void internarHandler(void){
	switch(internalControlGetEvent()){
	case OK_EVENT:

		if(fsm == ID_STAGE){

			if(validateID((id_txt.array)+TEXT_TAB)){
				fsm = PIN_STAGE;
				DispClear();
			}else{
				fsm = ID_ERROR_STAGE;
				t_switch = false;
				PIT_startTime(3);
				RGBIndicator(RED_INDICATOR);
				DispClear();
			}

		}else if(fsm == PIN_STAGE){
			if(validateUser((id_txt.array)+TEXT_TAB, (pin_txt.array)+TEXT_TAB)){
				fsm = CHECKOUT_STAGE;
				DispClear();
				t_switch = true;
				PIT_startTime(3);
				RGBIndicator(GREEN_INDICATOR);
			}else{
				fsm = ERROR_STAGE;
				DispClear();
				t_switch = false;
				PIT_startTime(3);
				RGBIndicator(RED_INDICATOR);
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
	PIT_stopTime(3);
	RGBIndicator(BLUE_INDICATOR);
	fsm = ID_STAGE;
	DispClear();
	clearUserInfo();
}

void accessDenied(void){
	PIT_stopTime(3);
	RGBIndicator(BLUE_INDICATOR);
	fsm = ID_STAGE;
	DispClear();
	clearUserInfo();
}



/* Task Start */
#define TASKSTART_STK_SIZE 		512u
#define TASKSTART_PRIO 			2u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];

/* Task 2 */
#define TASK2_STK_SIZE			256u
#define TASK2_STK_SIZE_LIMIT	(TASK2_STK_SIZE / 10u)
#define TASK2_PRIO              3u
static OS_TCB Task2TCB;
static CPU_STK Task2Stk[TASK2_STK_SIZE];

/* Example semaphore */
static OS_SEM semTest;

static void Task2(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    while (1) {
        OSSemPost(&semTest, OS_OPT_POST_1, &os_err);
        OSTimeDlyHMSM(0u, 0u, 0u, 200u, OS_OPT_TIME_HMSM_STRICT, &os_err);
        //LED_R_TOGGLE();
    }
}


static void TaskStart(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    /* Initialize the uC/CPU Services. */
    CPU_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    /* (optional) Compute CPU capacity with no task running */
    OSStatTaskCPUUsageInit(&os_err);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    /* Create semaphore */
    OSSemCreate(&semTest, "Sem Test", 0u, &os_err);

    /* Create Task2 */
    OSTaskCreate(&Task2TCB, 			//tcb
                 "Task 2",				//name
                  Task2,				//func
                  0u,					//arg
                  TASK2_PRIO,			//prio
                 &Task2Stk[0u],			//stack
                  TASK2_STK_SIZE_LIMIT,	//stack limit
                  TASK2_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    while (1) {
        OSTimeDlyHMSM(0u, 0u, 0u, 100u, OS_OPT_TIME_HMSM_STRICT, &os_err);
        //LED_G_TOGGLE();
    }
}

int main(void) {
    OS_ERR err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif

    hw_Init();

    aux_id = default_id; // Asignacion de ID por default

	initUser();

	PIT_init();

	internalControlInit(internarHandler);

	encoderInit(encoderHandler);

	PIT_configTimer(0, 10000, displayHandler);

	PIT_configTimer(3, 200000000,  timerSwitch);

	DispBoard_Init();

	DispClear();

	lectorInit(lectorHandler);

	RGBIndicator(BLUE_INDICATOR);

	PIT_startTime(0);

    OSInit(&err);

 #if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	 /* Enable task round robin. */
	 OSSchedRoundRobinCfg((CPU_BOOLEAN)1, 0, &err);
 #endif
    OS_CPU_SysTickInit(SystemCoreClock / (uint32_t)OSCfg_TickRate_Hz);

    OSTaskCreate(&TaskStartTCB,
                 "App Task Start",
                  TaskStart,
                  0u,
                  TASKSTART_PRIO,
                 &TaskStartStk[0u],
                 (TASKSTART_STK_SIZE / 10u),
                  TASKSTART_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 &err);

    OSStart(&err);

	/* Should Never Get Here */
    while (1) {

    }
}
