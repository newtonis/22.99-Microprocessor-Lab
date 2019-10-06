/*
 * SPI_driver.c
 *
 *  Created on: Oct 2, 2019
 *      Author: joa-m
 */
#include "MK64F12.h"
#include "spi.h"
//#include "board.h"

static SPI_Type *spiPtrs[] = SPI_BASE_PTRS;
static PORT_Type * ports[] = PORT_BASE_PTRS;
static uint32_t sim_port[] = {SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTE_MASK};

void SPI_driver_init (void){
    SPI_Type * spi_mod = spiPtrs[0];
    setup_pin(PIN_SCK);
    setup_pin(PIN_MOSI);
    setup_pin(PIN_MISO);
    setup_pin(PIN_CS);

    SIM->SCGC6 |= SIM_SCGC6_SPI0(1); //habilite el clock del periferico de spi

    spi_mod->MCR=SPI_MCR_HALT_MASK|SPI_MCR_MSTR_MASK;

    spi_mod->CTAR[0] = SPI_CTAR_CPOL(0) |  \
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
    spi_mod->MCR=(spi_mod->MCR & (~SPI_MCR_PCSIS_MASK))|SPI_MCR_PCSIS(1);
    spi_mod->MCR =(spi_mod->MCR & (~(SPI_MCR_MDIS_MASK | SPI_MCR_HALT_MASK | SPI_MCR_MSTR_MASK))) \
														| SPI_MCR_MDIS(0) |SPI_MCR_HALT(0) | SPI_MCR_MSTR(1);

}

void setup_pin (pin_t pin){
    int pinPort = PIN2PORT(pin);
    int pinBit = PIN2NUM(pin);
    SIM->SCGC5 |= sim_port[pinPort]; //Habilito el clock al puerto correspondiente

    //configuro el pcr del pin
    ports[pinPort]->PCR[pinBit]= 	PORT_PCR_MUX(SPI_DRIVER_MODE) | \
									PORT_PCR_IRQC(0) | \
									PORT_PCR_PS(0)| \
									PORT_PCR_PE(0);
}


uint8_t SPI_driver_sendRecive(uint8_t * data2end, uint8_t size,uint8_t * recivedData){
    SPI_Type * spi_mod = spiPtrs[0];
    uint32_t pushr2send=0;
    uint8_t dataRecived=0;

    for(int i=0;i<size;i++){
        if(i==(size-1)){//estoy en el ultimo dato a mandar
            pushr2send=SPI_PUSHR_PCS(1)|SPI_PUSHR_CONT(0)|SPI_PUSHR_EOQ(1)|SPI_PUSHR_TXDATA(data2end[i]);
        }else{ //no es el ultimo dato que mando
            pushr2send=SPI_PUSHR_PCS(1)|SPI_PUSHR_CONT(1)|SPI_PUSHR_TXDATA(data2end[i]);
        }

        spi_mod->MCR =(spi_mod->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(1);

        spi_mod->SR=(spi_mod->SR & (~SPI_SR_TCF_MASK) ) | SPI_SR_TCF(1) ; //reinicio el tcf
        spi_mod->PUSHR=pushr2send;

        spi_mod->MCR =(spi_mod->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(0);
        while(!(spi_mod->SR & SPI_SR_TCF_MASK));//espero a que se envie el frame

        if(recivedData!=NULL){
            if(spi_mod->SR & SPI_SR_RXCTR_MASK){
                recivedData[dataRecived]=spi_mod->POPR;
                dataRecived++;

            }
        }else{
            spi_mod->POPR;
        }
    }
    return dataRecived;
}
