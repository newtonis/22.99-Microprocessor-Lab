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

// DEFINIR EL I2C POINTER Y LO DEMAS, LE PONGO ALGO PROVISORIO:
// EL .c TIENE QUE PODER VER I2C.h
/////////////////////////////////////
typedef int8_t* MQX_FILE_PTR;
#define	I2C_OK		1
#define I2C_ERROR	0
int8_t s_i2c_read_regs(MQX_FILE_PTR pf, int8_t slave_addr, int8_t slave_id, uint8_t *data, uint8_t ack);
int8_t s_i2c_write_regs(MQX_FILE_PTR pf, int8_t slave_addr, int8_t slave_id, uint8_t *data, uint8_t ack);
/////////////////////////////////////

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief configures the FXOS8700CQ for 200-Hz hybrid mode meaning that both
 * accelerometer and magnetometer data are provided at the 200-Hz rate.
 * @param I2C pointer.
 * @return I2C_OK or I2C_ERROR.
 */
bool _mqx_ints_FXOS8700CQ_start(MQX_FILE_PTR aFP);

/**
 * @brief reads data from both sensors.
 * @param pointer to acceleration_data and to magnetometer_data.
 * @return I2C_OK or I2C_ERROR.
 */
bool ReadAccelMagnData(SRAWDATA *pAccelData, SRAWDATA *pMagnData);

#endif /* SENSORES_H_ */
