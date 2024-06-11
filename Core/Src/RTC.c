/*
 * RTC.c
 *
 *  Created on: Apr 15, 2024
 *      Author: kccistc
 */
#include "RTC.h"

I2C_HandleTypeDef *myHi2c1;
DateTime_t dateTime;

void initRTC(UART_HandleTypeDef *inHi2c1) {
	myHi2c1 = inHi2c1;
}
uint8_t BCD2Decimal(uint8_t inData) {
	uint8_t upper = inData >> 4;
	uint8_t lower = inData & 0x0f;
	return upper * 10 + lower;
}

uint8_t Decimal2BCD(uint8_t inData) {
	uint8_t upper = inData / 10;
	uint8_t lower = inData % 10;
	return upper << 4 | lower;
}

void setRTC(DateTime_t inData) {
	uint8_t txBuffer[8];
	txBuffer[7] = Decimal2BCD(inData.year);
	txBuffer[6] = Decimal2BCD(inData.month);
	txBuffer[5] = Decimal2BCD(inData.date);
	txBuffer[3] = Decimal2BCD(inData.hour);
	txBuffer[2] = Decimal2BCD(inData.min);
	txBuffer[1] = Decimal2BCD(inData.sec);
	txBuffer[0] = 0;
	HAL_I2C_Master_Transmit(myHi2c1, RTC_ADD, txBuffer, sizeof(txBuffer), 10);
}

DateTime_t getRTC() {
	DateTime_t result;
	uint8_t rxBuffer[7];
	uint8_t address = 0;
	HAL_I2C_Master_Transmit(myHi2c1, RTC_ADD, &address, 1, 10);
	HAL_I2C_Master_Receive(myHi2c1, RTC_ADD | READ, rxBuffer, 7, 10);
	result.year = BCD2Decimal(rxBuffer[6]);
	result.month = BCD2Decimal(rxBuffer[5]);
	result.date = BCD2Decimal(rxBuffer[4]);
	result.day = BCD2Decimal(rxBuffer[3]);
	result.hour = BCD2Decimal(rxBuffer[2]);
	result.min = BCD2Decimal(rxBuffer[1]);
	result.sec = BCD2Decimal(rxBuffer[0]);
	return result;
}

//address = 0 ~ 0x37
void writeRAM(uint8_t address, uint8_t data) {
	HAL_I2C_Mem_Write(myHi2c1, RTC_ADD, address, 1, &data, 1, 10);
}

uint8_t readRam(uint8_t address) {
	uint8_t result;
	HAL_I2C_Mem_Read(myHi2c1, RTC_ADD, address, 1, &result, 1, 10);
	return result;
}

void writeEEPROM(uint16_t address, uint8_t data) {
	HAL_I2C_Mem_Write(myHi2c1, ROM_ADD, address, 1, &data, 1, 10);
	HAL_Delay(5);
}

uint8_t readEEPROM(uint16_t address) {
	uint8_t result;
	HAL_I2C_Mem_Read(myHi2c1, ROM_ADD, address, 1, &result, 1, 10);
	return result;
}

void write2ByteEEPROM(uint16_t address, uint16_t data) {
	HAL_I2C_Mem_Write(myHi2c1, ROM_ADD, address, 2, &data, 2, 10);
	HAL_Delay(10);
}

uint16_t read2ByteEEPROM(uint16_t address) {
	uint16_t result;
	HAL_I2C_Mem_Read(myHi2c1, ROM_ADD, address, 2, &result, 2, 10);
	return result;

}

void write4ByteEEPROM(uint16_t address, uint16_t data) {
	HAL_I2C_Mem_Write(myHi2c1, ROM_ADD, address, 2, &data, 4, 10);
	HAL_Delay(20);
}

uint32_t read4ByteEEPROM(uint16_t address) {
	uint32_t result;
	HAL_I2C_Mem_Read(myHi2c1, ROM_ADD, address, 2, &result, 4, 10);
	return result;
}

void timeinit() {
	if (MagicNumber != read4ByteEEPROM(eeMagicNumberBase)) {
		//초기 ?��?��
		dateTime.year = 24;
		dateTime.month = 3;
		dateTime.date = 29;
		dateTime.hour = 14;
		dateTime.min = 29;
		dateTime.sec = 0;
		setRTC(dateTime);
		//매직?���?? 기록
		write4ByteEEPROM(eeMagicNumberBase, MagicNumber);
	}
}

void timeLCDprint() {
	dateTime = getRTC();

	char STR1[100];
	sprintf(STR1, "20%02d-%02d-%02d %02d:%02d\n", dateTime.year, dateTime.month,
			dateTime.date, dateTime.hour, dateTime.min);

	I2C_CLCD_GotoXY(0, 0);
	I2C_CLCD_PutStr(STR1);

	I2C_CLCD_GotoXY(1, 0);
}
