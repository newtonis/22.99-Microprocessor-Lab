/*
 * DisplayBoard.h
 *
 *  Created on: 30 Aug 2019
 *      Author: Marcelo
 */
#ifndef DISPLAYBOARD_H_
#define DISPLAYBOARD_H_

#include "board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define ID_LEN			8
#define PIN_LEN			5

#define DISP_LEN		4

#define ID_WORD_LEN		11
#define PIN_WORD_LEN	8

#define PIN_MENU		1
#define ID_MENU			2

#define END_LINE		DISP_LEN+1
#define DISP_TEST		{8, 8, 8, 8}
#define ID_TEST			{I_CHAR, D_CHAR, ESPACIO, 8, 8, 8, 8, 8, 8, 8, 8}
#define PIN_TEST		{P_CHAR, I_CHAR, N_CHAR, 0, 0, 0, 0, 0}
#define FLASH_TIME		40

/***** CHARACTER defines ******************************************************/
#define ESPACIO	-1
#define GUION	-2
#define DP		-3
#define I_CHAR	105
#define D_CHAR	100
#define P_CHAR	112
#define	N_CHAR	110

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void DispBoard_Init(void);

void DispShowID(int id[ID_LEN]);

void DispShowPIN(int pin[PIN_LEN]);

void DispShiftMsj(int menu_case);

int DispGetCursor(void);

void DispClear(void);

void Status_Write(int code);

#endif /* DISPLAYBOARD_H_ */

