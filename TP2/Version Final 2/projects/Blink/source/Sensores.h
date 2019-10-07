/*
 * Sensores.h
 *
 *  Created on: 26 Sep 2019
 *      Author: Marcelo
 */

#ifndef SENSORES_H_
#define SENSORES_H_

#include <stdint.h>
#include <hardware.h>


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
} SRAWDATA;

typedef void (* callbackp) (void);

typedef enum { I2C_ERROR,  I2C_OK} I2C_FAIL;

typedef struct
{
	SRAWDATA * pMagnData;
	SRAWDATA * pAccelData;
	callbackp callback;
	I2C_FAIL error;
}read_data;




/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief configures the FXOS8700CQ for 200-Hz hybrid mode meaning that both
 * accelerometer and magnetometer data are provided at the 200-Hz rate.
 * @param I2C pointer.
 * @return I2C_OK or I2C_ERROR.
 */
I2C_FAIL _mqx_ints_FXOS8700CQ_start(void);

/**
 * @brief reads data from both sensors.
 * @param pointer to acceleration_data and to magnetometer_data.
 * @return I2C_OK or I2C_ERROR.
 */
void ReadAccelMagnData(read_data * data);

#endif /* SENSORES_H_ */