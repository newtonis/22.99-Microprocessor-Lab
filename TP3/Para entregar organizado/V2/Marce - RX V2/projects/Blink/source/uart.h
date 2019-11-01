/***************************************************************************//**
  @file     UART.c
  @brief    UART Driver for K64F. Non-Blocking and using FIFO feature
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _UART_H_
#define _UART_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UART_CANT_IDS   1
#define UART_BUFFER_SIZE 100
#define TERMINADOR '\n' // El terminador separa las palabras!


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	uint32_t baudrate;
} uart_cfg_t;

//struct uart_manager{
//	uint8_t send[UART_BUFFER_SIZE]; // buffer to send
//	uint8_t recv[UART_BUFFER_SIZE]; // buffer received
//	int actual_send;
//	int actual_recv;
//
//	void (*onWordRecv)(char *word);
//
//} uart3_manager;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize UART driver
 * @param id UART's number
 * @param config UART's configuration (baudrate, parity, etc.)
*/
void uartInit(uart_cfg_t config);
void sendWord(const char *word);

void getNextWord(char *data);

int getNextWordLength();
char * getWord();
void popWord();

void updateWord();
void setOnNewCharListener(void *recv(char));

//void byteRecv(uart_manager *uart, uint8_t data);

/**
 * @brief Check if a new byte was received
 * @param id UART's number
 * @return A new byte has being received
*/
//uint8_t uartIsRxMsg(uart_manager* uart, uint8_t id);

/**
 * @brief Check how many bytes were received
 * @param id UART's number
 * @return Quantity of received bytes
*/
//uint8_t uartGetRxMsgLength(uart_manager* uart, uint8_t id);

/**
 * @brief Read a received message. Non-Blocking
 * @param id UART's number
 * @param msg Buffer to paste the received bytes
 * @param cant Desired quantity of bytes to be pasted
 * @return Real quantity of pasted bytes
*/
//uint8_t uartReadMsg(uart_manager* uart, uint8_t id, char* msg, uint8_t cant);

/**
 * @brief Write a message to be transmitted. Non-Blocking
 * @param id UART's number
 * @param msg Buffer with the bytes to be transfered
 * @param cant Desired quantity of bytes to be transfered
 * @return Real quantity of bytes to be transfered
*/
//uint8_t uartWriteMsg(uart_manager *uart, uint8_t id, const char* msg, uint8_t cant);

/**
 * @brief Check if all bytes were transfered
 * @param id UART's number
 * @return All bytes were transfered
*/
//uint8_t uartIsTxMsgComplete(uart_manager *uart, uint8_t id);


/*******************************************************************************
 ******************************************************************************/

#endif // _UART_H_
