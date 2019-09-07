/*
 * DisplayBoard.h
 *
 *  Created on: 30 Aug 2019
 *      Author: Marcelo
 */
#ifndef DISPLAYBOARD_H_
#define DISPLAYBOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define SIZEOFARR(a)	sizeof(a)/sizeof(a[0])

#define DISP_LEN		4

#define MENU_TYPE		0
#define NUM_TYPE		1

#define END_LINE		DISP_LEN+1
#define DISP_TEST		{8, 8, 8, 8}
#define FLASH_TIME		160
#define SHIFT_TIME		500

#define MIN_BRIGHT		2
#define MAX_BRIGHT		20

/***** CHARACTER defines ******************************************************/
#define ESPACIO	-1
#define GUION	-2
#define DP		-3
#define A_CHAR	97
#define C_CHAR	99
#define D_CHAR	100
#define E_CHAR	101
#define I_CHAR	105
#define L_CHAR	108
#define	N_CHAR	110
#define O_CHAR	111
#define P_CHAR	112
#define R_CHAR	114
#define S_CHAR	115
#define U_CHAR	117


typedef uint8_t disp_cursor_t;
typedef uint8_t disp_bright_t;
typedef	bool disp_task_t;

typedef struct {
	int                 array[30];
	int                 len;
    int                 prefix_len;
    int					sufix_len;
} disp_msj_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void DispBoard_Init(void);

void DispShowMsj(disp_msj_t msj);

void DispShiftMsj(void);

disp_cursor_t DispGetCursor(void);

disp_task_t DispModType(void);

disp_bright_t DispChangeBright(int move_dir);

/**
 * @brief Clears display text, must call always when menu changes

 */
void DispClear(void);

void Status_Write(int code);

#endif /* DISPLAYBOARD_H_ */

