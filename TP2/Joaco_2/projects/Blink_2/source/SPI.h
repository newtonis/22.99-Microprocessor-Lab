/*
 * SPI.h
 *
 *  Created on: 18 sep. 2019
 *      Author: joa-m
 */

#ifndef SPI_H_
#define SPI_H_


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "board.h"
//#include "hardware.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define BUFF_LEN 100
//#define PIN2PORT(p)         (((p)>>5) & 0x07)
//#define LOW     0
//#define HIGH    1
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef uint8_t pin_t;
enum{GPIO_ALTERNATIVE=1,SPI_ALTERNATIVE};
enum{SLAVE,MASTER};
enum{DISABLE,ENABLE};
enum{PULL_DOWN,PULL_UP,DISABLE_PULL};
enum{SPI_0,SPI_1,SPI_2};
enum{SPI_CONFIG,RESERVED_0,RESERVED_1,RESERVED_2};

typedef enum which_pcs_config{
	Pcs0 = (1U << 0) , /*!< Pcs[0] */
	Pcs1 = (1U << 1) , /*!< Pcs[1] */
	Pcs2 = (1U << 2) , /*!< Pcs[2] */
	Pcs3 = (1U << 3) , /*!< Pcs[3] */
	Pcs4 = (1U << 4) , /*!< Pcs[4] */
	Pcs5 = (1U << 5)   /*!< Pcs[5] */
}pcs_t;


typedef struct{

	uint32_t CTAR_CPOL;
	uint32_t CTAR_CPHA;
	uint32_t CTAR_PBR;
	uint32_t CTAR_BR;
	uint32_t CTAR_FMSZ;
	uint32_t CTAR_ASC;
	uint32_t CTAR_PASC;
	uint32_t CTAR_CSSCK;
	uint32_t CTAR_PCSSCK;
	uint32_t CTAR_PDT;
	uint32_t CTAR_DT;

} spi_master_config_t;

typedef struct{
	bool whichCTAR;

	bool MCR_isMaster;
	bool MCR_ContinousSerialClockEnable; // habilita el clock a correr continuamente
	bool MCR_ModifiedTransferFormatEnable; // habilita el modo de ModifiedTransfer
	bool MCR_ReceiveFIFOverflowOverwriteEnable; // Rxfifo llena y sigue llegando datos: 0 data es ignorada, 1 se shiftea data
	uint8_t MCR_Sample_Point;// configuro cuando sampleo en Modified Transfer Format, solo valido cuando CTAR[CPHA]=0
	bool MCR_isTxfifoDisabled; // Desactivo Txfifo con 1
	bool MCR_isRxfifoDisabled; // Desactivo Rxfifo con 1
	bool MCR_isModuleDisabled; // Desactivar modulo, con 0 habilito, con 1 deshabilito


	uint8_t CTAR_FrameSize;
	bool CTAR_CPOL; //estado del clock en IDLE
	bool CTAR_CPHA; // seteo en que flanco muestreo, 0 es en el primero, 1 en el segundo
	bool CTAR_LSBFE;
	uint8_t CTAR_BR;
	uint8_t CTAR_BRPRESC;
	uint8_t CTAR_DelayAfterTransferScaler; // quiero que sea 2ms para 200hz
	uint8_t CTAR_DelayAfterTransferPrescaler;
	uint8_t CTAR_DelayAfterSCKtoNegPCSScaler;
	uint8_t CTAR_DelayAfterSCKtoNegPCSPrescaler;
	uint8_t CTAR_DelayPCStoSCKScaler;
	uint8_t CTAR_DelayPCStoSCKPrescaler;
}MSTRCFG_t;



typedef struct{

	bool keepAssertedPCSnBetweenTransfers;
	bool whichCtar;
	uint32_t whichPcs;
	bool isEndOfQueue;
	bool clearTransferCount;

}spi_command;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief sets Master Mode
 * @param the number of SPI
 * @return void
 */
//void setMasterMode(uint8_t SPI_n);


void setPinConfigurations(uint8_t SPI_n)

void masterConfig(spi_master_config_t * master_cfg);

void masterInitiliaze(uint8_t SPI_n);

void SPI_Initialize(void (*funcallback)(void));

void SPIClockGatingEnable(uint8_t SPI_n);

void configPin(pin_t pin, uint8_t mode, uint8_t gpio_mode, uint8_t mux_alt, uint8_t interrupt_alt);

void testSPI(uint8_t SPI_n);



void setSPIConfig(uint8_t SPI_n, int SPI_config);


void disableTxFIFO(uint8_t SPI_n);

void enableTxFIFO(uint8_t SPI_n);

void disableRxFIFO(uint8_t SPI_n);

void enableRxFIFO(uint8_t SPI_n);




void clearTxFIFOCounter(uint8_t SPI_n); //flush

void clearRxFIFOCounter(uint8_t SPI_n); //flush




void HALTStopTransfers(uint8_t SPI_n);

void HALTStartTransfers(uint8_t SPI_n);


void setPCSActiveHigh(uint8_t SPI_n);

void setPCSActiveLow(uint8_t SPI_n);



void clearTxCompleteFlag(uint8_t SPI_n);

void clearTxFifoFillRequestFlag(uint8_t SPI_n);

uint16_t masterWriteReadByte(uint8_t SPI_n, spi_command *command, uint16_t data);


/*
void clearInterruptFlag(pin_t pin);


void clearAllSPIInterruptFlags(void);

void setInterruptConfig(pin_t pin, isf_configs_t config);


void configAllSPIInterrupts(void);

void spiEnableInterrupts(uint8_t SPI_n, uint32_t mask);

*/

/*******************************************************************************
 ******************************************************************************/


#endif /* SPI_H_ */
