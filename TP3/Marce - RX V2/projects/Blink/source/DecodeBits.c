/*
 * DecodeBits.c
 *
 *  Created on: 27 Oct 2019
 *      Author: Marcelo
 */

#include "DecodeBits.h"
#include "hardware.h"
#include "DMA.h"
//#include "ftm.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

enum{NOT_TAKEN_SYMBOLS, FIRST_DC_TAKEN, SECOND_DC_TAKEN};

static uint8_t buffer[11] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
static uint32_t input_pulse = 0;
static uint32_t entradasleidas[22] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t cont = 0;

static uint32_t med, med1, med2;
static uint32_t medStatus = 0;

static uint8_t index = 0;
static bool is_buff_full = false;

static uint8_t  status = NOT_TAKEN_SYMBOLS;
static uint8_t a0;
static uint8_t a1;

static bool is_RX = false; // En IDLE por default

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
bool isNumberN(uint8_t N, uint8_t n, uint8_t threshold);

bool isZero(uint8_t n, uint8_t threshold);

bool isValidA0(uint8_t input,uint8_t thresh_hold);

bool isValidA1(uint8_t a0,uint8_t input,uint8_t thresh_hold);

void bufferFlush(void);

void buffer2UART(void);

void writeBuffer(uint8_t a0, uint8_t a1, uint8_t threshold);

bool isBufferFull(void);

void write2BuffLogic(uint8_t a0, uint8_t a1, uint8_t threshold);

void decodeDutys(uint8_t input);

void processPulse(void);

void Decoder_parsePulse(uint32_t input);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void Decoder_init(uint32_t* medAddress)
{
	DMA1_Config(&(FTM3->CONTROLS[5].CnV), &input_pulse, processPulse);

	DMA0_ConfigCounters(1, sizeof(input_pulse), sizeof(input_pulse));

	DMA0_EnableRequest(1); // habilita el request de channel 1 de DMA0
}

void processPulse(void)
{
	if(medStatus == 0)
	{
		med1 = input_pulse & FTM_CnV_VAL_MASK; // Primer Flanco
		medStatus = 1;
	}
	else if(medStatus == 1)
	{
		med2 = input_pulse & FTM_CnV_VAL_MASK; // Segundo Flanco

		if(med2 > med1){
			med = med2 - med1;
		}
		else{
			med = med1 - med2;
		}


		//med = med + (cnt_ovf*0xFFFF);
		medStatus = 0;					// Set break point here and watch "med" value

		Decoder_parsePulse(med);
		entradasleidas[cont] = med;
		cont++;
		if(cont == 22)
		{
			cont = 0;
		}
	}
}

void Decoder_parsePulse(uint32_t pulse_in)
{
	if((pulse_in < 400)||(pulse_in > 20000))
	{
		// Filtrado de glitches
	}
	else
	{
		if(pulse_in > 12000)
		{
			decodeDutys(100);
			decodeDutys(0);
		}
		else
		{
			decodeDutys(50);
		}

		if(is_RX)
		{

		}
		else
		{
			// Espero a recibir un bit de start
			if(index == 1)
			{
				if(buffer[0] == 0)
				{
					// Es un bit de start
					is_RX = true;
				}
				else
				{
					// En otro caso reseteo y sigo esperando
					bufferFlush();
					index = 0;
				}
			}

		}

	}

}


bool isNumberN(uint8_t N, uint8_t n, uint8_t threshold){
    if( (N-threshold)<=n && n<=(N+threshold) ){
        return true;
    }else{
        return false;
    }
}
bool isZero(uint8_t n, uint8_t threshold){
    if(n<=threshold){
        return true;
    }else{
        return false;
    }
}


bool isValidA0(uint8_t input,uint8_t thresh_hold){
    if(isNumberN(100,input,thresh_hold)){
        return true;
    }else if(isNumberN(50,input,thresh_hold)) {
        return true;
    }else{
        return false;
    }
}

bool isValidA1(uint8_t a0,uint8_t input,uint8_t thresh_hold){
    if( isNumberN(100,a0,thresh_hold) && isZero(input,thresh_hold)){
        return true;
    }else if( isNumberN(50,a0,thresh_hold) && isNumberN(50,input,thresh_hold)) {
        return true;
    }else{
        return false;
    }
}

void bufferFlush(void){
    for(int i = 0; i < sizeof(buffer) ; i++) {
        buffer[i] = 2;
    }
    is_buff_full = false;
}

void buffer2UART(void){ // esto debería ser un callback

    is_RX = false;
}

void writeBuffer(uint8_t a0, uint8_t a1, uint8_t threshold){
    if( isNumberN(100,a0,threshold) &&  isZero(a1,threshold)){
        buffer[index] = 1;
    }else if( isNumberN(50,a0,threshold) && isNumberN(50,a1,threshold) ){
        buffer[index] = 0;
    }
    if(index == (sizeof(buffer)-1) ){
        is_buff_full = true;
    }
    index ++;
    index %= sizeof(buffer);
}

bool isBufferFull(void){
    return is_buff_full;
}

void write2BuffLogic(uint8_t a0, uint8_t a1, uint8_t threshold){
    if(!isBufferFull()){
        writeBuffer(a0,a1,threshold);
    }else{
        buffer2UART();
        bufferFlush();
        writeBuffer(a0,a1,threshold);
    }
}

void decodeDutys(uint8_t input){
    if(status==NOT_TAKEN_SYMBOLS){
        if(isValidA0(input,10)){
            a0 = input;
            status = FIRST_DC_TAKEN;
        }
    }else if(status == FIRST_DC_TAKEN){
        if(isValidA1(a0,input,10)){
            a1 = input;
            status = SECOND_DC_TAKEN;
        }else{
            if(isNumberN(100,input,10)) { // si era valido a0 pero es el seugndo caracter
                if(isNumberN(50,a0, 10)){ // si de 50 pasa a 100, entonces antes era 50,50 y ahora agarro 100
                    write2BuffLogic(50,50,10);
                }
                a0 = 100;
                status = FIRST_DC_TAKEN;
            }else if(isNumberN(50, input, 10)){ // si de 0 pasa a 50
                if(isZero(a0,10)){              // asumo que nates hubo un 100, 0 y ahora viene
                    write2BuffLogic(100,0,10);  //
                }
                a0 = 50;
                status = FIRST_DC_TAKEN;
            }else{
                // si fue totalmente invalido el input predigo qué va a venir
                if(isNumberN(100,a0,10)){
                    a1= 0;
                }else if(isNumberN(50,a0,10)){
                    a1= 50;
                }
                status = SECOND_DC_TAKEN;
            }
        }
    }
    if(status == SECOND_DC_TAKEN){
        write2BuffLogic(a0,a1,10);
        status = NOT_TAKEN_SYMBOLS;
    }
}

/*******************************************************************************
 ******************************************************************************/
