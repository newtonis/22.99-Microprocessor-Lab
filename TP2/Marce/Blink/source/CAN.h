/*
 * CAN.h
 *
 *  Created on: 2 Oct 2019
 *      Author: Marcelo
 */

#ifndef CAN_H_
#define CAN_H_

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void init_CAN(int ID);

void send_CAN(int ID, char * buffer, int bufflen);

#endif /* CAN_H_ */
