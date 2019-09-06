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
static int id_vector[ID_WORD_LEN] = ID_TEST;
static int pin_vector[PIN_WORD_LEN] = PIN_TEST;
static int bright_vector[BRIGHT_LEN] = {L_CHAR, U_CHAR, 1, 0};

static bool load_menu = 0;
static int flash_counter = 0;
static int cursor_flash = 0;	// Para ver donde estoy modificando
static int brightness = 20;		// Inicialmente maximo brillo
static int bright_time = 0;
static int vector_cont = 0;
static bool shift_hab = 0;
static bool flash_fix = 1;
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

void Disp7Seg_Write(int sym){
	switch(sym){
	case DP:
		gpioWrite(segments[7], HIGH);
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


void DispShowMsj(int *number_arr, int len){

	if(!load_menu){ // Se hace la primera vez solamente
		for(int k = 0; k < DISP_LEN; k++){
			switch (len) {
			case PIN_LEN:
				Status_Write(2);
				disp_vector[k] = pin_vector[k];
				flash_fix = 0;
				break;
			case ID_LEN:
				Status_Write(1);
				disp_vector[k] = id_vector[k];
				flash_fix = 1;
				break;
			case BRIGHT_LEN:
				Status_Write(3);
				disp_vector[k] = bright_vector[k];
				flash_fix = 1;
				break;
			}
		}
		load_menu = 1;
		vector_cont = DISP_LEN-1;
	}

	for(int i = 0; i < len; i++){
		switch(len){
		case PIN_LEN:
			pin_vector[i+3] = number_arr[i];
			if(i < DISP_LEN){
				disp_vector[i] = pin_vector[i+pin_pointer];
			}
			break;
		case ID_LEN:
			id_vector[i+3] = number_arr[i];
			if(i < DISP_LEN){
				disp_vector[i] = id_vector[i+pin_pointer];
			}
			break;
		case BRIGHT_LEN:
			if(i < DISP_LEN){
				disp_vector[i] = bright_vector[i];
			}
			break;
		}
	}
/*
	if(shift_counter == SHIFT_TIME){
		shift_counter = 0;

	    if(word_pos != 0){
		    for(int i = 0; i < DISP_LEN-1; i++)
		    {
		        disp_vector[i]=disp_vector[i+1];
		    }
		    disp_vector[DISP_LEN-1] = pin_vector[vector_cont];
		    vector_cont++;
		    word_pos--;
		    if(word_pos == 0){
		    	pin_pointer = 0;
		    }
	    }
	}
*/

	if(!(flash_counter%FLASH_TIME)){
		cursor_flash++;
	}

	Disp7Seg_Select(dsp);

	// Aca vendria la parte mistica del brillo
	if(bright_time <= brightness){

		if(vector_cont == DISP_LEN-1){ // Parpadeo para cambio de menu
			if(dsp < DISP_LEN-flash_fix){
				if(cursor_flash % 2){
					Disp7Seg_Write(ESPACIO);
				}else{
					Disp7Seg_Write(disp_vector[dsp-1]);
				}
			}else{
				Disp7Seg_Write(disp_vector[dsp-1]);
			}
		}else{ // Parpadeo para cambio de numero

			if(len == BRIGHT_LEN){
				if(dsp > DISP_LEN-2){
					if(cursor_flash % 2){
						Disp7Seg_Write(ESPACIO);
					}else{
						Disp7Seg_Write(disp_vector[dsp-1]);
					}
				}else{
					Disp7Seg_Write(disp_vector[dsp-1]);
				}
			}else{
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

void DispShiftMsj(int menu_case){

	switch(menu_case){
	case PIN_MENU:
	    if(vector_cont == PIN_WORD_LEN){
	    	vector_cont = DISP_LEN-1;
	    	shift_hab = 0;
	    	pin_pointer = 0;
	    	for(int k = 0; k < DISP_LEN; k++){
	    		disp_vector[k] = pin_vector[k];
	    	}
	    }else{
	        for(int i = 0; i < DISP_LEN-1; i++)
	        {
	            disp_vector[i]=disp_vector[i+shift_hab];
	        }
	        disp_vector[DISP_LEN-1] = pin_vector[vector_cont];
	        vector_cont++;
	    	if(!shift_hab){
	    		shift_hab = 1;
	    	}else{
	    		pin_pointer++;
	    	}
	    }
		break;
	case ID_MENU:
	    if(vector_cont == ID_WORD_LEN){
	    	vector_cont = DISP_LEN-1;
	    	shift_hab = 0;
	    	pin_pointer = 0;
	    	for(int k = 0; k < DISP_LEN; k++){
	    		disp_vector[k] = id_vector[k];
	    	}
	    }else{
	        for(int i = 0; i < DISP_LEN-1; i++)
	        {
	            disp_vector[i]=disp_vector[i+shift_hab];
	        }
	        disp_vector[DISP_LEN-1] = id_vector[vector_cont];
	        vector_cont++;
	    	if(!shift_hab){
	    		shift_hab = 1;
	    	}else{
	    		pin_pointer++;
	    	}
	    }
		break;
	case BRIGHT_MENU:
	    if(vector_cont == DISP_LEN){
	    	vector_cont = DISP_LEN-1;
	    	shift_hab = 0;
	    }else{
	        vector_cont++;
	    	if(!shift_hab){
	    		shift_hab = 1;
	    	}
	    }
		break;
	}

}

void DispClear(void){
	for(int i = 0; i < 4; i++){
		disp_vector[i] = ESPACIO;
	}
	load_menu = 0;
	pin_pointer = 0;
}

disp_cursor_t DispGetCursor(void){
	return pin_pointer;
}

disp_task_t DispModType(void){
	return shift_hab;
}

void DispChangeBright(int move_dir){
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

	if(brightness != MAX_BRIGHT){
		bright_vector[2] = 0;
		bright_vector[3] = brightness/2;
	}else{
		bright_vector[2] = 1;
		bright_vector[3] = 0;
	}
}

/*******************************************************************************
 ******************************************************************************/
