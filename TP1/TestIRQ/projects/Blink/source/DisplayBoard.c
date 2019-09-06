/*
 * DisplayBoard.c
 *
 *  Created on: 30 Aug 2019
 *      Author: Marcelo
 */
#include "DisplayBoard.h"
#include "Encoder.h"
#include "board.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static int segments[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
static int disp_selector[] = {SEL0, SEL1};
static int status_leds[] = {STAT0, STAT1};

static int disp_vector[DISP_LEN] = DISP_TEST;

static bool load_menu = 0;
static int flash_counter = 0;
static int cursor_flash = 0;	// Para ver donde estoy modificando
static int brightness = 20;		// Inicialmente maximo brillo
static int bright_time = 0;
static int vector_cont = 0;
static int actual_len = 0;
static int actual_prefix = 0;
static bool roll_end = 0;

static bool shift_hab = 0;
static int pin_pointer = 0;
static int dsp = 1;

/*******************************************************************************
                        LOCAL FUNCTION DECLARATIONS
 ******************************************************************************/
void Disp7Seg_Write(int sym);

bool Disp7Seg_Select(int disp);

/*******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 ******************************************************************************/

void DispBoard_Init(void){
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
	Disp7Seg_Select(1); // Default choice
	Disp7Seg_Write(8);
	Disp7Seg_Write(DP);
	Status_Write(1);
}


bool Disp7Seg_Select(int disp){
	switch(disp){
	case 1:
		gpioWrite(disp_selector[0], LOW);
		gpioWrite(disp_selector[1], LOW);
		return 1;
	case 2:
		gpioWrite(disp_selector[0], HIGH);
		gpioWrite(disp_selector[1], LOW);
		return 1;
	case 3:
		gpioWrite(disp_selector[0], LOW);
		gpioWrite(disp_selector[1], HIGH);
		return 1;
	case 4:
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


void DispShowMsj(int *msj_arr, int len, int prefix_len){

	if(!load_menu){ // Se hace la primera vez solamente por nuevo mensaje
		for(int k = 0; k < DISP_LEN; k++){
			disp_vector[k] = msj_arr[k];
			vector_cont = 0;
			actual_len = len;
			actual_prefix = prefix_len;
		}
		load_menu = 1;
	}

	for(int i = 0; i < DISP_LEN; i++){
		disp_vector[i] = msj_arr[i+pin_pointer];
	}

	if(!(flash_counter%FLASH_TIME)){
		cursor_flash++;
	}

	Disp7Seg_Select(dsp);
	// HASTA ACA ESTARIA ///////////////////////////

	// Aca vendria la parte mistica del brillo
	if(bright_time <= brightness){

		if(vector_cont == 0){ // Parpadeo para cambio de menu
			if(dsp <= prefix_len){
				if(cursor_flash % 2){
					Disp7Seg_Write(ESPACIO);
				}else{
					Disp7Seg_Write(disp_vector[dsp-1]);
				}
			}else{
				Disp7Seg_Write(disp_vector[dsp-1]);
			}
		}else{ // Parpadeo para cambio de numero

			if(dsp == DISP_LEN){
				if(cursor_flash % 2){
					Disp7Seg_Write(ESPACIO);
				}else{
					Disp7Seg_Write(disp_vector[dsp-1]);
				}
			}else{
				Disp7Seg_Write(disp_vector[dsp-1]);
			}

		}

	}else{
		Disp7Seg_Write(ESPACIO);
	}


	bright_time++;
	if(bright_time == MAX_BRIGHT){
		bright_time = 0;
		dsp++;
	}

	if(dsp == END_LINE){
		dsp = 1;
	}

	flash_counter++;
}

void DispShiftMsj(void){

    if(vector_cont == actual_len){
    	vector_cont = 0;
    	shift_hab = 0;
    	pin_pointer = 0;
    }else{
    	if(vector_cont == 0){
    		shift_hab = 1;
    	}else{
    		pin_pointer++;
    	}
        vector_cont++;
    }

}

void DispClear(void){
	for(int i = 0; i < 4; i++){
		disp_vector[i] = ESPACIO;
	}
	load_menu = 0;
	pin_pointer = 0;
	roll_end = 0;
}

disp_cursor_t DispGetCursor(void){
	return pin_pointer;
}

disp_task_t DispModType(void){
	return shift_hab;
}

disp_bright_t DispChangeBright(int move_dir){
	switch(move_dir){
	case RIGHT:
		brightness+=MIN_BRIGHT;
		if(brightness > MAX_BRIGHT){
			brightness = MIN_BRIGHT;
		}
		break;
	case LEFT:
		brightness-=MIN_BRIGHT;
		if(brightness < MIN_BRIGHT){
			brightness = MAX_BRIGHT;
		}
		break;
	default:
		// Nada por defecto
		break;
	}
	return brightness;
}

bool DispCheckRollMsj(void){
	return roll_end;
}

void Disp7Seg_Write(int sym){
	switch(sym){
	case DP:
		gpioWrite(segments[7], HIGH);
		break;
	case A_CHAR:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], LOW);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case C_CHAR:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], LOW);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case E_CHAR:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], LOW);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case S_CHAR:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], LOW);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case O_CHAR:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case L_CHAR:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], LOW);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], LOW);
		gpioWrite(segments[7], LOW);
		break;
	case U_CHAR:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], LOW);
		gpioWrite(segments[7], LOW);
		break;
	case D_CHAR:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], HIGH);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case I_CHAR:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], LOW);
		gpioWrite(segments[4], LOW);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], LOW);
		gpioWrite(segments[7], LOW);
		break;
	case P_CHAR:
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], LOW);
		gpioWrite(segments[3], LOW);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case N_CHAR:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], LOW);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case GUION:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], LOW);
		gpioWrite(segments[3], LOW);
		gpioWrite(segments[4], LOW);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], HIGH);
		gpioWrite(segments[7], LOW);
		break;
	case ESPACIO:
		gpioWrite(segments[0], LOW);
		gpioWrite(segments[1], LOW);
		gpioWrite(segments[2], LOW);
		gpioWrite(segments[3], LOW);
		gpioWrite(segments[4], LOW);
		gpioWrite(segments[5], LOW);
		gpioWrite(segments[6], LOW);
		gpioWrite(segments[7], LOW);
		break;
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
/*******************************************************************************
 ******************************************************************************/
