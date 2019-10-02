/*
 * Posicionamiento.c
 *
 *  Created on: 20 Sep 2019
 *      Author: Marcelo
 */
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "Posicionamiento.h"
#include "Sensores.h"
#include <math.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum{CLOCKWISE, COUNTER_CLOCKWISE, ZERO};

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
SRAWDATA accel_cords = {.x = -1, .y = -1, .z = -1};  // Aca guardar las coordenadas
SRAWDATA magnet_cords = {.x = 0, .y = -1, .z = 0}; //

roll_t roll = 0; 		// Form -179° to 180°
pitching_t pitching = 0;	// Form -179° to 180°
orientation_t orientation = 0;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void Position_CalculateRoll(void);
void Position_CalculatePitching(void);
void Position_CalculateOrientation(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void Position_InitDrv(void){
	// Inicializar los sensores
	//_mqx_ints_FXOS8700CQ_start(ACA VA EL PUNTERO);
}

void Position_Update(void){
	ReadAccelMagnData(&accel_cords, &magnet_cords);
}

roll_t Position_GetRoll(void){
	Position_CalculateRoll();
	return roll;
}

pitching_t Position_GetPitch(void){
	Position_CalculatePitching();
	return pitching;
}

orientation_t Position_GetOrientation(void){
	Position_CalculateOrientation();
	return orientation;
}

void Position_CalculateRoll(void){
	uint8_t clock_type;
	uint8_t tita;

	if(accel_cords.x > 0){
		clock_type = CLOCKWISE;
	}else if(accel_cords.x < 0){
		clock_type = COUNTER_CLOCKWISE;
	}else{
		clock_type = ZERO;
	}

	if(clock_type != ZERO){
		float aux = sqrtf(powf(accel_cords.z, 2) + powf(accel_cords.y, 2));
		tita = 180*atan(aux/fabs(accel_cords.x))/M_PI;
	}else{
		tita = 90;
	}

	switch (clock_type) {
		case CLOCKWISE:
			if(accel_cords.z < 0){
				roll = - 90 + tita;
			}else if(accel_cords.z > 0){
				roll = - 90 - tita;
			}else{
				roll = - 90;
			}
			break;
		case COUNTER_CLOCKWISE:
			if(accel_cords.z < 0){
				roll = 90 - tita;
			}else if(accel_cords.z > 0){
				roll = 90 + tita;
			}else{
				roll = 90;
			}
			break;
		case ZERO:
			if(accel_cords.z < 0){
				roll = 0;
			}else if(accel_cords.z > 0){
				roll = 180; // Por defecto, podria ser tambien
			}
			break;
	}
}

void Position_CalculatePitching(void){
	uint8_t clock_type;
	uint8_t tita;

	if(accel_cords.y > 0){
		clock_type = CLOCKWISE;
	}else if(accel_cords.y < 0){
		clock_type = COUNTER_CLOCKWISE;
	}else{
		clock_type = ZERO;
	}

	if(clock_type != ZERO){
		float aux = sqrtf(powf(accel_cords.z, 2) + powf(accel_cords.x, 2));
		tita = 180*atan(aux/fabs(accel_cords.y))/M_PI;
	}else{
		tita = 90;
	}

	switch (clock_type) {
		case CLOCKWISE:
			if(accel_cords.z < 0){
				pitching = - 90 + tita;
			}else if(accel_cords.z > 0){
				pitching = - 90 - tita;
			}else{
				pitching = - 90;
			}
			break;
		case COUNTER_CLOCKWISE:
			if(accel_cords.z < 0){
				pitching = 90 - tita;
			}else if(accel_cords.z > 0){
				pitching = 90 + tita;
			}else{
				pitching = 90;
			}
			break;
		case ZERO:
			if(accel_cords.z < 0){
				pitching = 0;
			}else if(accel_cords.z > 0){
				pitching = 180; // Por defecto, podria ser tambien
			}
			break;
	}
}

void Position_CalculateOrientation(void){
	uint8_t clock_type;
	uint8_t tita;

	if(magnet_cords.y > 0){
		clock_type = CLOCKWISE;
	}else if(magnet_cords.y < 0){
		clock_type = COUNTER_CLOCKWISE;
	}else{
		clock_type = ZERO;
	}

	if(clock_type != ZERO){
		float aux = sqrtf(powf(magnet_cords.y, 2) + powf(magnet_cords.z, 2));
		tita = 180*atan(fabs(magnet_cords.x)/aux)/M_PI;
	}else{
		tita = 90;
	}

	switch (clock_type) {
		case CLOCKWISE:
			if(magnet_cords.x > 0){
				orientation = 180 - tita;
			}else if(magnet_cords.x < 0){
				orientation = - 179 + tita;
			}else{
				orientation = 180;
			}
			break;
		case COUNTER_CLOCKWISE:
			if(magnet_cords.x < 0){
				orientation = 0 - tita;
			}else if(magnet_cords.x > 0){
				orientation = tita;
			}else{
				orientation = 0;
			}
			break;
		case ZERO:
			if(magnet_cords.x < 0){
				orientation = 0 - tita;
			}else if(magnet_cords.x > 0){
				orientation = tita; // Por defecto, podria ser tambien
			}
			break;
	}
}

/*******************************************************************************
 ******************************************************************************/
