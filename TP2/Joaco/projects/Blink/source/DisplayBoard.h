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

#define MENU_TYPE		0
#define NUM_TYPE		1

/***** CHARACTER defines ******************************************************/
#define ESPACIO	-1
#define GUION	-2
#define DP		-3
#define A_CHAR	97
#define C_CHAR	99
#define D_CHAR	100
#define E_CHAR	101
#define H_CHAR	104
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

/**
 * @brief initializes display board
 */
void DispBoard_Init(void);

/**
 * @brief shows the message msj, must be called externally every 20ms
 * @param message to show according to structure parameters
 */
void DispShowMsj(disp_msj_t msj);

/**
 * @brief shifts the actual message shown one step to the left
 */
void DispShiftMsj(void);

/**
 * @brief getter for the actual display cursor
 * @return pointer position referred to the numbers to modify -1
 * Example: third PIN number -> returns 2
 */
disp_cursor_t DispGetCursor(void);

/**
 * @brief getter for information of the actual modifier
 * @return 0 = MENU_TYPE is menu switching mode, 1 = NUM_TYPE number modify mode
 */
disp_task_t DispModType(void);

/**
 * @brief changes display brightness
 * @param RIGHT or LEFT
 * @return resulting bright in scale of 1 to 10, in steps of 1.
 */
disp_bright_t DispChangeBright(int move_dir);

/**
 * @brief clears display text, must call always when menu changes.
 */
void DispClear(void);

/**
 * @brief sets the active led on the display board
 * @param the number of the led to activate (1, 2 or 3).
 */
void LedStatus_Write(int code);

/**
 * @brief getter for the actual display board led status
 * @return 1, 2 or 3 for the corresponding leds in the board.
 */
int LedStatus_GetState(void);

#endif /* DISPLAYBOARD_H_ */

