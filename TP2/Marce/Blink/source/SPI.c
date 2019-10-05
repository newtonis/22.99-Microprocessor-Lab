/*
 * SPI_driver.c
 *
 *  Created on: Sep 20, 2019
 *      Author: gonzalosilva
 */
#include "board.h"
#include "MK64F12.h"
#include <stdio.h>

#define SPI_DRIVER_MODE		2 //alternativa
#define SPI_DRIVER_INPUT	1

#define SPI_DRIVER_BR 5
#define SPI_DRIVER_PBR 1

typedef uint8_t pin_t;

static bool trasmiting=false;

static SPI_Type *SPI_driver_SPI[] = {SPI0, SPI1, SPI2};
static void setup_pin (pin_t pin);

static PORT_Type * ports[] = PORT_BASE_PTRS;
static uint32_t sim_port[] = {SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTE_MASK};

static void startTrasmissionReception(void){
	SPI_driver_SPI[0]->MCR =(SPI_driver_SPI[0]->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(0);
}
static void stopTrasmissionReception(void){
	SPI_driver_SPI[0]->MCR =(SPI_driver_SPI[0]->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(1);
}


void SPI_driver_init (void){
	setup_pin(PIN_SCK);
	setup_pin(PIN_MOSI);
	setup_pin(PIN_MISO);
	setup_pin(PIN_CS);

	SIM->SCGC6 |= SIM_SCGC6_SPI0(1); //habilite el clock del periferico de spi

	SPI_driver_SPI[0]->MCR=SPI_MCR_HALT_MASK|SPI_MCR_MSTR_MASK;

	SPI_driver_SPI[0]->CTAR[0] = SPI_CTAR_CPOL(0) |  \
								 SPI_CTAR_CPHA(0)| \
								 SPI_CTAR_PBR(SPI_DRIVER_PBR) | \
								 SPI_CTAR_BR(SPI_DRIVER_BR) | \
								 SPI_CTAR_FMSZ(8-1)| \
								 SPI_CTAR_ASC(SPI_DRIVER_BR-3) | \
								 SPI_CTAR_PASC(SPI_DRIVER_PBR) | \
								 SPI_CTAR_CSSCK(SPI_DRIVER_BR-3) | \
								 SPI_CTAR_PCSSCK(SPI_DRIVER_PBR) |
								 SPI_CTAR_PDT(0)|\
								 SPI_CTAR_DT(0);
			//configuracion del SPI 0
	SPI_driver_SPI[0]->MCR=(SPI_driver_SPI[0]->MCR & (~SPI_MCR_PCSIS_MASK))|SPI_MCR_PCSIS(1);
	SPI_driver_SPI[0]->MCR =(SPI_driver_SPI[0]->MCR & (~(SPI_MCR_MDIS_MASK | SPI_MCR_HALT_MASK | SPI_MCR_MSTR_MASK))) \
														| SPI_MCR_MDIS(0) |SPI_MCR_HALT(0) | SPI_MCR_MSTR(1);
	//stopTrasmissionReception();

}

void setup_pin (pin_t pin){
	int pinPort = PIN2PORT(pin);
	int pinBit = PIN2NUM(pin);
	SIM->SCGC5 |= sim_port[pinPort]; //Habilito el clock al puerto correspondiente

	//configuro el pcr del pin
	ports[pinPort]->PCR[pinBit]= PORT_PCR_MUX(SPI_DRIVER_MODE) | \
			PORT_PCR_IRQC(0) | \
			PORT_PCR_PS(0)| \
			PORT_PCR_PE(0);


}


uint8_t SPI_driver_sendRecive(uint8_t * data2end, uint8_t size,uint8_t * recivedData){
	static uint32_t pushrInicial=SPI_PUSHR_PCS(1)|SPI_PUSHR_CONT(1); //cont 1
	static uint32_t pushrFinal=SPI_PUSHR_PCS(1)|SPI_PUSHR_CONT(0)|SPI_PUSHR_EOQ(1);//cont0 eoq
	uint32_t pushr2send=0;
	uint8_t dataRecived=0;
	//vajar todos los flags del sr

	for(int i=0;i<size;i++){
		if((i+1)==size){//estoy en el ultimo dato a mandar
			pushr2send=pushrFinal|SPI_PUSHR_TXDATA(data2end[i]);
		}else{ //no es el ultimo dato que mando
			pushr2send=pushrInicial|SPI_PUSHR_TXDATA(data2end[i]);
		}


		stopTrasmissionReception();
		SPI_driver_SPI[0]->SR=(SPI_driver_SPI[0]->SR & (~SPI_SR_TCF_MASK) ) | SPI_SR_TCF(1) ; //reinicio el tcf
		SPI_driver_SPI[0]->PUSHR=pushr2send;
		startTrasmissionReception();


		while(!(SPI_driver_SPI[0]->SR & SPI_SR_TCF_MASK));//espero a que se envie el frame

		//SPI_driver_SPI[0]->SR= (SPI_driver_SPI[0]->SR & ~SPI_SR_TCF_MASK)| SPI_SR_TCF_MASK; //si no anda borrame

		if(recivedData!=NULL){
			if(SPI_driver_SPI[0]->SR & SPI_SR_RXCTR_MASK){
				recivedData[dataRecived]=SPI_driver_SPI[0]->POPR;
				dataRecived++;

			}


		}else{
			SPI_driver_SPI[0]->POPR;
		}



	}


			//SPI_driver_SPI[0]->PUSHR|=SPI_PUSHR_PCS(1);
			//SPI_driver_SPI[0]->PUSHR=(SPI_driver_SPI[0]->PUSHR&(~(SPI_PUSHR_TXDATA_MASK | SPI_PUSHR_PCS_MASK)))| SPI_PUSHR_TXDATA(data2end[i])|SPI_PUSHR_PCS(1);
			//SPI_driver_SPI[0]->PUSHR=(SPI_driver_SPI[0]->PUSHR&(~(SPI_PUSHR_TXDATA_MASK)))| SPI_PUSHR_TXDATA(data2end[i]);

	return dataRecived;
}
bool SPI_driver_dataSended(void){
	if(trasmiting){
		if(SPI_driver_SPI[0]->SR & SPI_SR_TCF_MASK){
			SPI_driver_SPI[0]->SR = SPI_SR_TCF_MASK;//borro el flag
			trasmiting=false;
		}
	}
	return !trasmiting;
}
bool SPI_driver_availableDataRecived(void){
	return true;
}
uint8_t SPI_driver_getData(uint8_t * dataRecived){
	uint8_t amauntOfData=0;
	if(trasmiting){
		return 0;
	}

	amauntOfData=(SPI_driver_SPI[0]->SR & SPI_SR_RXCTR_MASK)>>SPI_SR_RXCTR_SHIFT;
	if(amauntOfData!=0){
		for(int i=0;i<amauntOfData;i++){
			dataRecived[i]=SPI_driver_SPI[0]->POPR;
		}
	}

	return amauntOfData;
}
//void SPI0_IRQHandler(void)
