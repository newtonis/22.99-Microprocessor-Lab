/*
 * ftm.h
 *
 *  Created on: 18 oct. 2019
 *      Author: joa-m
 */

#ifndef FTM_H_
#define FTM_H_

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

#define FTM_MODULE_NUMBER 0


__ISR__ FTM0_IRQHandler(void);

void OVF_ISR(void);

void ftmInit(void);



#endif /* FTM_H_ */
