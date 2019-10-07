/*
 * ExternalManager.h
 *
 *  Created on: 5 Oct 2019
 *      Author: Marcelo
 */

#ifndef EXTERNALMANAGER_H_
#define EXTERNALMANAGER_H_

#include <stdint.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
#define GROUPS	7

typedef int16_t roll_t;			// Rolido
typedef	int16_t pitching_t;		// Cabeceo
typedef int16_t orientation_t;	// Orientacion

typedef struct
{
	roll_t roll;
	pitching_t pitching;
	orientation_t orientation;
}BoardParams_t;

#define ROLL_UPD         1
#define PITCHING_UPD     2
#define ORIENTATION_UPD	 3

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void ExternManager_init(uint8_t group_num);

void ExternManager_send2Ext(BoardParams_t myBoard, uint8_t typeUPD);

#endif /* EXTERNALMANAGER_H_ */
