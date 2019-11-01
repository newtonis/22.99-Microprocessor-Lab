/*
 * demodulatorFSK.h
 *
 *  Created on: Oct 18, 2019
 *      Author: martinamaspero
 */

#ifndef DEMODULATORFSK_H_
#define DEMODULATORFSK_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


#define DELAY 5 //NUMBER OF SAMPLE PERIODS
#define FIR_ORDER 18

typedef void (*myCallback)(void);

void FSKdem_init( myCallback funcallback);
char get_Msg (void);
void FSKdemodulate(void);
bool isDataReady(void);





#endif /* DEMODULATORFSK_H_ */
