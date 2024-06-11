/*
 * Motor.c
 *
 *  Created on: Apr 12, 2024
 *      Author: kccistc
 */

#include "Motor.h"
#include <stdio.h>
STATUS_t state;
TIM_HandleTypeDef *myHtim;

void initMotor(TIM_HandleTypeDef *inHtim){
	myHtim = inHtim;
}

//Motor
void goForward(uint8_t percent) {
	HAL_GPIO_WritePin(GPIOC, esc_A_Pin, 1);
	HAL_GPIO_WritePin(GPIOC, esc_B_Pin, 0);
	myHtim->Instance->CCR1 = (percent*10)-1;
	state = FORWARD;
}
void goBackward(uint8_t percent) {
	HAL_GPIO_WritePin(GPIOC, esc_A_Pin, 0);
	HAL_GPIO_WritePin(GPIOC, esc_B_Pin, 1);
	myHtim->Instance->CCR1 = (percent*10)-1;
	state = BACKWARD;
}

void stopMove() {
	HAL_GPIO_WritePin(GPIOC, esc_A_Pin, 0);
	HAL_GPIO_WritePin(GPIOC, esc_B_Pin, 0);
	myHtim->Instance->CCR1 = 0;
	state = NONE;
}

void go(){
	HAL_GPIO_WritePin(GPIOC, esc_A_Pin, 1);
	HAL_GPIO_WritePin(GPIOC, esc_B_Pin, 0);
	for (int i = 75; i<=100;i++){
		myHtim->Instance->CCR1 = (i*10)-1;
		HAL_Delay(500);
	}
}
