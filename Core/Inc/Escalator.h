/*
 * Escalator.h
 *
 *  Created on: Apr 14, 2024
 *      Author: kccistc
 */

#ifndef INC_ESCALATOR_H_
#define INC_ESCALATOR_H_
#include "main.h"
#include "Motor.h"
#include "UltraSonic.h"
#include "uart.h"
#include "I2C_CLCD.h"
typedef enum{
	UP = 0,
	DOWN,
	STOP
}ESCALATOR_STATUS_t;

typedef enum{
	SLOW = 0,
	FAST
}SPEED_STATUS_t;

void SysTickcallback();

void EscalatorSpeedEventCheck();

void EscalatorStateCheck();
void EscalatorRUN();

void EscalatorStateCheck_FAST();
void EscalatorStateCheck_SLOW();

void EscalatorRUN_FAST();
void EscalatorRUN_SLOW();


#endif /* INC_ESCALATOR_H_ */
