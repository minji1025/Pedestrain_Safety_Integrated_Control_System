/*
 * UltraSonic.h
 *
 *  Created on: Apr 12, 2024
 *      Author: kccistc
 */

#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_
#include "main.h"

void initUltraSonic(TIM_HandleTypeDef *inHtim1,TIM_HandleTypeDef *inHtim11);
void delayUs(uint16_t time);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
uint32_t getDistance();
uint32_t getDistance2();


#endif /* INC_ULTRASONIC_H_ */
