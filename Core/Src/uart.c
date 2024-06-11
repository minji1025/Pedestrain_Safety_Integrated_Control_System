/*
 * uart.c
 *
 *  Created on: Mar 11, 2024
 *      Author: kccistc
 */

#include "uart.h"
#include <stdio.h>

#define rxBufferMax 255

int rxBufferGp;	//get pointer(read)
int rxBufferPp;	//put pointer(write)
uint8_t rxBuffer[rxBufferMax];
uint8_t rxChar;
UART_HandleTypeDef *myHuart;

//init device
void initUart(UART_HandleTypeDef *inHuart){
	myHuart = inHuart;
	HAL_UART_Receive_IT(myHuart, &rxChar, 1);
}

//process received charactor
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	rxBuffer[rxBufferPp++] = rxChar;
	rxBufferPp %= rxBufferMax;
	HAL_UART_Receive_IT(myHuart, &rxChar, 1);
}

//get charactor from buffer
uint8_t getChar(){
	uint8_t result;
	if(rxBufferGp == rxBufferPp) return 0;
	result = rxBuffer[rxBufferGp++];
	rxBufferGp %= rxBufferMax;
	return result;
}

char *getMessage(){
	static char msgBuffer[100];
	static int pos = 0;
	char ch = getChar();
	if(ch != 0){
		switch(ch){
			case '\n':
				memset(msgBuffer,0,100);
				pos = 0;
				break;
			default:
				msgBuffer[pos++] = ch;
				break;
		}
	}
	return msgBuffer;
}

int _write(int file, char *p, int len) {
	HAL_UART_Transmit(myHuart, p, len, 10);
	return len;
}


