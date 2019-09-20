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
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum{DISABLE,ENABLE};
enum{PULL_DOWN,PULL_UP,DISABLE_PULL};
enum{SPI_0,SPI_1,SPI_2};
enum{SLAVE,MASTER};
enum{SPI_CONFIG,RESERVED_0,RESERVED_1,RESERVED_2};
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
void setMasterMode(int SPI_n);
/**
 * @brief sets Slave Mode
 * @param the number of SPI
 * @return void
 */
void setSlaveMode(int SPI_n);
/**
 * @brief gets which Mode is activated
 * @param the number of SPI
 * @return bool indicating the mode
 */

bool getMode(int SPI_n);

/**
 * @brief Enables Continous Clock SCK
 * @param void
 * @return bool indicating the mode
 */

void initSPIDriver(void);

void enableContinousSCK(int SPI_n);

void disableContinousSCK(int SPI_n);

void setSPIConfig(int SPI_n, int SPI_config);

void enableFreeze(int SPI_n);

void disableFreeze(int SPI_n);

void enableModifyTransferFormat(int SPI_n);

void disableModifyTransferFormat(int SPI_n);

void enableChipSelectStrobe(int SPI_n);

void disableChipSelectStrobe(int SPI_n);

void enableRxFIFOOverflowOverwrite(int SPI_n);

void disableRxFIFOOverflowOverwrite(int SPI_n);

void dozeEnable(int SPI_n);

void dozeDisable(int SPI_n);

void moduleDisable(int SPI_n);

void moduleEnable(int SPI_n);

void disableTxFIFO(int SPI_n);

void enableTxFIFO(int SPI_n);

void disableRxFIFO(int SPI_n);

void enableRxFIFO(int SPI_n);

void clearTxFIFO(int SPI_n); //flush

void dontClearTxFIFO(int SPI_n);

void clearRxFIFO(int SPI_n); //flush

void dontClearRxFIFO(int SPI_n);

void HALTStopTransfers(int SPI_n);

void HALTStartTransfers(int SPI_n);


/*
void sendTxFifoInfo(tx_info_t data);
rx_info_t getReceivedRxFifoInfo(void);
void disableTxFifo(void); // esto es para low latency updates to SPI queues
void disableRxFifo(void); // esto es para low latency updates to SPI queues
*/

//void addDMAEntryToTxFifo(dma_info_t data);

//void removeDMAEntryFromRxFifo(dma_info_t data);

// Interrupciones:
// EOQF = End of Queue Reached
// TFFF = Tx fifo is not full
// TCF = Transfer of current frame complete
// TFUF = Attemp to transmit with an empty Transmit FIFO
// RFDF = RX fifo is not empty
// RFOF = Frame received while Receive FIFO is full (RFOF)

// SPI tiene multiples interrupciones, las fuentes de esas interrupciones
// son OReadas para generar una interrupción simple al controlador de interrupciones
// cuando ocurren hay que leer SPI_SR para determinar la interrupción exacta

/*******************************************************************************
 ******************************************************************************/


#endif /* SPI_H_ */
