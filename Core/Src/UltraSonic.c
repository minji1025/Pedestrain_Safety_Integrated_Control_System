/*
 * UltraSonic.c
 *
 *  Created on: Apr 12, 2024
 *      Author: kccistc
 */
#include "UltraSonic.h"

TIM_HandleTypeDef *myHtim1;
TIM_HandleTypeDef *myHtim11;
uint32_t IC_Val1_Channel1 = 0;
uint32_t IC_Val2_Channel1 = 0;
uint32_t IC_Val1_Channel3 = 0;
uint32_t IC_Val2_Channel3 = 0;
uint32_t Difference1 = 0;
uint32_t Difference3 = 0;
uint8_t IsFirstCapture1 = 0;
uint8_t IsFirstCapture3 = 0;
uint32_t Distance1 = 0;
uint32_t Distance3 = 0;

void initUltraSonic(TIM_HandleTypeDef *inHtim1, TIM_HandleTypeDef *inHtim11) {
	myHtim1 = inHtim1;
	myHtim11 = inHtim11;
}
void delayUs(uint16_t time) {
	myHtim11->Instance->CNT = 0;
	while (myHtim11->Instance->CNT < time);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		if (IsFirstCapture1 == 0) {
			IC_Val1_Channel1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			IsFirstCapture1 = 1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
					TIM_INPUTCHANNELPOLARITY_FALLING);
		} else if (IsFirstCapture1 == 1) {
			IC_Val2_Channel1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			htim->Instance->CNT = 0;
			if (IC_Val2_Channel1 > IC_Val1_Channel1)
				Difference1 = IC_Val2_Channel1 - IC_Val1_Channel1;
			else if (IC_Val1_Channel1 > IC_Val2_Channel1)
				Difference1 = (0xffff - IC_Val1_Channel1) + IC_Val2_Channel1;
			Distance1 = Difference1 * 0.034 / 2;
			IsFirstCapture1 = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
					TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
		}
	}
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
		if (IsFirstCapture3 == 0) {
			IC_Val1_Channel3 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			IsFirstCapture3 = 1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3,
					TIM_INPUTCHANNELPOLARITY_FALLING);
		} else if (IsFirstCapture3 == 1) {
			IC_Val2_Channel3 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			htim->Instance->CNT = 0;
			if (IC_Val2_Channel3 > IC_Val1_Channel3)
				Difference3 = IC_Val2_Channel3 - IC_Val1_Channel3;
			else if (IC_Val1_Channel3 > IC_Val2_Channel3)
				Difference3 = (0xffff - IC_Val1_Channel3) + IC_Val2_Channel3;
			Distance3 = Difference3 * 0.034 / 2;
			IsFirstCapture3 = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3,
					TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_IT_CC3);
		}
	}
}

uint32_t getDistance() {
	//trigger
	HAL_GPIO_WritePin(trigger_GPIO_Port, trigger_Pin, 1);
	delayUs(10);
	HAL_GPIO_WritePin(trigger_GPIO_Port, trigger_Pin, 0);

	__HAL_TIM_ENABLE_IT(myHtim1, TIM_IT_CC1);
	return Distance1;
}
uint32_t getDistance2() {
	//trigger
	HAL_GPIO_WritePin(trigger2_GPIO_Port, trigger2_Pin, 1);
	delayUs(10);
	HAL_GPIO_WritePin(trigger2_GPIO_Port, trigger2_Pin, 0);

	__HAL_TIM_ENABLE_IT(myHtim1, TIM_IT_CC3);
	return Distance3;
}
