/*
 * Motor.h
 *
 *  Created on: Apr 12, 2024
 *      Author: kccistc
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"

typedef enum{
	NONE = 0,
	FORWARD,
	BACKWARD,
	TURNLEFT,
	TURNRIGHT,
	SAFE
}STATUS_t;

void initMotor(TIM_HandleTypeDef *inHtim);
void goForward(uint8_t percent);
void goBackward(uint8_t percent);
void stopMove();

#endif /* INC_MOTOR_H_ */
