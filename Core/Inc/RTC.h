/*
 * RTC.h
 *
 *  Created on: Apr 15, 2024
 *      Author: kccistc
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "main.h"
#define RTC_ADD     0xD0
#define ROM_ADD     0xA0
#define READ        1
#define MagicNumber 0x257fad14
// eeprom map table
#define eeMagicNumberBase 0
#define eeMagicNumberSize 4

typedef struct {
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} DateTime_t;

#endif /* INC_RTC_H_ */
