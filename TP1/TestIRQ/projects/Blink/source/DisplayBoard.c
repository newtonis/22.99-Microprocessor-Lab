/*
 * DisplayBoard.c
 *
 *  Created on: 30 Aug 2019
 *      Author: Marcelo
 */
#include "DisplayBoard.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static int segments[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
static int disp_selector[] = {SEL0, SEL1};
static int status_leds[] = {STAT0, STAT1};

static int disp_vector[DISP_LEN] = DISP_TEST;
static int id_vector[ID_WORD_LEN] = ID_TEST;
static int pin_vector[PIN_WORD_LEN] = PIN_TEST;

static int shift_counter = 0;
static int dsp = 1;
static int n_dsp = 4;

/*******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 ******************************************************************************/


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
}

void Disp7Seg_Write(int sym){
	switch(sym){
	case DP:
		gpioWrite(segments[7], HIGH);
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
		gpioWrite(segments[1], HIGH);
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
		gpioWrite(segments[0], HIGH);
		gpioWrite(segments[1], HIGH);
		gpioWrite(segments[2], HIGH);
		gpioWrite(segments[3], LOW);
		gpioWrite(segments[4], HIGH);
		gpioWrite(segments[5], HIGH);
		gpioWrite(segments[6], LOW);
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

void DispShowID(int id[ID_LEN]){
	static int id_counter = 0;

	for(int k = 0; k < ID_LEN; k++){
		id_vector[k+3] = id[k];
	}

	if(shift_counter == SHIFT_TIME){
		shift_counter = 0;
	    for(int i = 0; i < DISP_LEN-1; i++)
	    {
	        disp_vector[i]=disp_vector[i+1];
	    }

	    if(id_counter == ID_WORD_LEN){
	    	disp_vector[DISP_LEN-1] = ESPACIO;
	    	id_counter = 0;
	    }else{
		    disp_vector[DISP_LEN-1] = id_vector[id_counter];
		    id_counter++;
	    }
	}

	Disp7Seg_Select(dsp);
	Disp7Seg_Write(disp_vector[dsp-1]);

	// Dibujo el punto a la mitad del ID
	if((id_counter >= (ID_WORD_LEN-DISP_LEN))&&(id_counter <= ID_WORD_LEN)){
		if(n_dsp == (id_counter-(ID_WORD_LEN-DISP_LEN-1))){
			Disp7Seg_Write(DP);
		}
	}

	dsp++;
	n_dsp--;
	if(dsp == END_LINE){
		dsp = 1;
		n_dsp = 4;
	}

	shift_counter++;
}

void DispShowPIN(int pin[PIN_LEN]){
	for(int k = 0; k < DISP_LEN; k++){
		disp_vector[k] = pin_vector[k];
	}

	Disp7Seg_Select(dsp);
	Disp7Seg_Write(disp_vector[dsp-1]);
	dsp++;
	if(dsp == END_LINE){
		dsp = 1;
	}
}

/*******************************************************************************
 ******************************************************************************/
