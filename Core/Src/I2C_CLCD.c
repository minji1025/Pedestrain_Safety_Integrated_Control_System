#include "I2C_CLCD.h"

I2C_HandleTypeDef *myHi2c;
I2C_HandleTypeDef *myHi2c2;

void initCLCD(I2C_HandleTypeDef *inHi2c1,I2C_HandleTypeDef*inHi2c2){
	myHi2c = inHi2c1;
	myHi2c2 = inHi2c2;
	I2C_CLCD_CustomFont_UPARROW2();
}
void I2C_CLCD_Delay_us(uint8_t us) {
	volatile uint8_t i;

	for (i = 0; i < 19 * us / 10; i++)
		;
}

void I2C_CLCD_SendByte(uint8_t RS_State, uint8_t Byte) {
	uint8_t i;
	uint8_t buffer[4];

	for (i = 0; i < 2; i++) {
		buffer[i] = (Byte & 0xf0) | (1 << I2C_CLCD_LED) | (!i << I2C_CLCD_E)
				| (0 << I2C_CLCD_RW) | (RS_State << I2C_CLCD_RS);
	}

	for (i = 0; i < 2; i++) {
		buffer[i + 2] = (Byte << 4) | (1 << I2C_CLCD_LED) | (!i << I2C_CLCD_E)
				| (0 << I2C_CLCD_RW) | (RS_State << I2C_CLCD_RS);
	}
	HAL_I2C_Master_Transmit(myHi2c, PCF8574_AD | WRITE, buffer, 4, 300);

	I2C_CLCD_Delay_us(40);
}

void I2C_CLCD_init(void) {
	uint8_t i;
	uint8_t CLCD_Init_CMD[4] = { 0x28, 0x0c, 0x01, 0x06 };

	HAL_Delay(100);

	I2C_CLCD_SendByte(0, 0x02);

	HAL_Delay(2);

	for (i = 0; i < 4; i++) {
		I2C_CLCD_SendByte(0, CLCD_Init_CMD[i]);

		if (i == 2)
			HAL_Delay(2);
	}
}

void I2C_CLCD_GotoXY(uint8_t X, uint8_t Y) {
	I2C_CLCD_SendByte(0, 0x80 | (0x40 * Y + X));
}

void I2C_CLCD_PutC(uint8_t C) {
	if (C == '\f') {
		I2C_CLCD_SendByte(0, 0x01);
		HAL_Delay(2);
	} else if (C == '\n') {
		I2C_CLCD_GotoXY(0, 1);
	} else {
		I2C_CLCD_SendByte(1, C);
	}
}

void I2C_CLCD_PutStr(uint8_t *Str) {
	while (*Str)
		I2C_CLCD_PutC(*Str++);
}

void I2C_CLCD_Cursor(uint8_t on) {
	I2C_CLCD_SendByte(0, (0x0c | (on << 1)));
}

void I2C_CLCD_Right() {
	I2C_CLCD_SendByte(0, 0b00011100);
}

void I2C_CLCD_Left() {
	I2C_CLCD_SendByte(0, 0b00011000);
}

void I2C_CLCD_ShiftStop() {
	I2C_CLCD_SendByte(0, 0b0001000);
}

void I2C_CLCD_CustomFont() { //set CGRAM
	I2C_CLCD_SendByte(0, 0x40); //첫번째 폰트
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte(1, 0b00000);
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte(1, 0b10000);
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte(1, 0b11000);
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte(1, 0b11100);
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte(1, 0b11110);
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte(1, 0b11111);
}

void 	I2C_CLCD_Progressbar(uint8_t n,uint8_t line){
	I2C_CLCD_GotoXY(0,line);
	for(int i = 0; i < (n/5);i++)
		I2C_CLCD_PutC(5);
	I2C_CLCD_PutC(n % 5);
	for(int i = (n/5)+1; i<16 ;i++)
		I2C_CLCD_PutC(0);
}
//CGBuffer[0],CGBuffer[1],CGBuffer[2],CGBuffer[3],CGBuffer[4],CGBuffer[5],CGBuffer[6],CGBuffer[7],
// 5 x 8 = 40bit
uint64_t CGBuffer[8];


void I2C_CLCD_CG_ClearBuffer(){
	memset(CGBuffer,0,8*8*8); //512byte= 64byte * 8줄
}

void I2C_CLCD_CG_ScrollLeft(){
	for(int i = 0; i < 8; i++){
		CGBuffer[i] = CGBuffer[i]<<1;
	}
}

void I2C_CLCD_CG_DrawPixel(uint8_t x, uint8_t y){	//20번째(x)에 조도값(y)에 점을 찍음
	CGBuffer[y] |= 1 << (39-x);
}

void I2C_CLCD_CG_Update(){
	I2C_CLCD_SendByte(0,0x40);
	for(int j = 0; j < 8;j++){ //CGRAM0 ~ CGRAM7(8글자)
		for(int i =0; i < 8;i++){	//한글자당 8줄
			I2C_CLCD_SendByte(1,CGBuffer[i]>>(35-j*5));
		}
	}
}

/*CGBuffer[0],CGBuffer[1],CGBuffer[2],CGBuffer[3],
 	CGBuffer[4],CGBuffer[5],CGBuffer[6],CGBuffer[7],
 	0000 0000 0000 [0000 0][000 00][00 000][0 0000] =20 bit
 		12bit는 버림			0				1				2				3

uint32_t CGBuffer[16];

void I2C_CLCD_CG_ClearBuffer(){
	memset(CGBuffer,0,64);
}

void I2C_CLCD_CG_ScrollLeft(){
	for(int i = 0; i <16; i++){
		CGBuffer[i] = CGBuffer[i]<<1;
	}
}

void I2C_CLCD_CG_DrawPixel(uint8_t x, uint8_t y){	//20번째(x)에 조도값(y)에 점을 찍음
	CGBuffer[y] |= 1 << (19-x);
}

void I2C_CLCD_CG_Update(){
	I2C_CLCD_SendByte(0,0x40);
	for(int j = 0; j < 4;j++){ //CGRAM0 ~ CGRAM3
		for(int i =0; i < 8;i++){	//8줄을 한 줄씩 그려줌
			I2C_CLCD_SendByte(1,CGBuffer[i]>>(15-j*5));
		}
	}
	for(int j = 0; j < 4;j++){
			for(int i =0; i < 8;i++){	//CGRAM4 ~ CGRAM7
				I2C_CLCD_SendByte(1,CGBuffer[i+8]>>(15-j*5));
			}
		}
}
 */

void I2C_CLCD_SendByte2(uint8_t RS_State, uint8_t Byte) {
	uint8_t i;
	uint8_t buffer[4];

	for (i = 0; i < 2; i++) {
		buffer[i] = (Byte & 0xf0) | (1 << I2C_CLCD_LED) | (!i << I2C_CLCD_E)
				| (0 << I2C_CLCD_RW) | (RS_State << I2C_CLCD_RS);
	}

	for (i = 0; i < 2; i++) {
		buffer[i + 2] = (Byte << 4) | (1 << I2C_CLCD_LED) | (!i << I2C_CLCD_E)
				| (0 << I2C_CLCD_RW) | (RS_State << I2C_CLCD_RS);
	}
	HAL_I2C_Master_Transmit(myHi2c2, PCF8574_AD | WRITE, buffer, 4, 300);

	I2C_CLCD_Delay_us(40);
}

void I2C_CLCD_init2(void) {
	uint8_t i;
	uint8_t CLCD_Init_CMD[4] = { 0x28, 0x0c, 0x01, 0x06 };

	HAL_Delay(100);

	I2C_CLCD_SendByte2(0, 0x02);

	HAL_Delay(2);

	for (i = 0; i < 4; i++) {
		I2C_CLCD_SendByte2(0, CLCD_Init_CMD[i]);

		if (i == 2)
			HAL_Delay(2);
	}
}

void I2C_CLCD_GotoXY2(uint8_t X, uint8_t Y) {
	I2C_CLCD_SendByte2(0, 0x80 | (0x40 * Y + X));
}

void I2C_CLCD_PutC2(uint8_t C) {
	if (C == '\f') {
		I2C_CLCD_SendByte2(0, 0x01);
		HAL_Delay(2);
	} else if (C == '\n') {
		I2C_CLCD_GotoXY2(0, 1);
	} else {
		I2C_CLCD_SendByte2(1, C);
	}
}

void I2C_CLCD_PutStr2(uint8_t *Str) {
	while (*Str)
		I2C_CLCD_PutC2(*Str++);
}

void I2C_CLCD_Cursor2(uint8_t on) {
	I2C_CLCD_SendByte2(0, (0x0c | (on << 1)));
}

void I2C_CLCD_Right2() {
	I2C_CLCD_SendByte2(0, 0b00011100);
}

void I2C_CLCD_Left2() {
	I2C_CLCD_SendByte2(0, 0b00011000);
}

void I2C_CLCD_ShiftStop2() {
	I2C_CLCD_SendByte2(0, 0b0001000);
}

void I2C_CLCD_CustomFont2() { //set CGRAM
	I2C_CLCD_SendByte2(0, 0x40); //첫번째 폰트
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte2(1, 0b00000);
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte2(1, 0b10000);
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte2(1, 0b11000);
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte2(1, 0b11100);
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte2(1, 0b11110);
	for(int i = 0; i<8 ;i++)
		I2C_CLCD_SendByte2(1, 0b11111);
}

void 	I2C_CLCD_Progressbar2(uint8_t n,uint8_t line){
	I2C_CLCD_GotoXY2(0,line);
	for(int i = 0; i < (n/5);i++)
		I2C_CLCD_PutC2(5);
	I2C_CLCD_PutC2(n % 5);
	for(int i = (n/5)+1; i<16 ;i++)
		I2C_CLCD_PutC2(0);
}
//CGBuffer[0],CGBuffer[1],CGBuffer[2],CGBuffer[3],CGBuffer[4],CGBuffer[5],CGBuffer[6],CGBuffer[7],
// 5 x 8 = 40bit
uint64_t CGBuffer2[8];


void I2C_CLCD_CG_ClearBuffer2(){
	memset(CGBuffer2,0,8*8*8); //512byte= 64byte * 8줄
}

void I2C_CLCD_CG_ScrollLeft2(){
	for(int i = 0; i < 8; i++){
		CGBuffer2[i] = CGBuffer2[i]<<1;
	}
}

void I2C_CLCD_CG_DrawPixel2(uint8_t x, uint8_t y){	//20번째(x)에 조도값(y)에 점을 찍음
	CGBuffer2[y] |= 1 << (39-x);
}

void I2C_CLCD_CG_Update2(){
	I2C_CLCD_SendByte2(0,0x40);
	for(int j = 0; j < 8;j++){ //CGRAM0 ~ CGRAM7(8글자)
		for(int i =0; i < 8;i++){	//한글자당 8줄
			I2C_CLCD_SendByte2(1,CGBuffer2[i]>>(35-j*5));
		}
	}
}

void I2C_CLCD_Clear2()
{
   I2C_CLCD_SendByte2(0, 0b00000001);
}

void I2C_CLCD_CustomFont_UPARROW2()
{
   I2C_CLCD_SendByte2(0, 0x40);      // set CGRAM address(0b0100 0000)

   I2C_CLCD_SendByte2(1, 0b00000);   // PutC(0)
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00001);
   I2C_CLCD_SendByte2(1, 0b00011);
   I2C_CLCD_SendByte2(1, 0b00111);
   I2C_CLCD_SendByte2(1, 0b01111);
   I2C_CLCD_SendByte2(1, 0b11111);

   I2C_CLCD_SendByte2(1, 0b00000);   // PutC(1)
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b11111);

   I2C_CLCD_SendByte2(1, 0b11111);   // PutC(2)
   I2C_CLCD_SendByte2(1, 0b01111);
   I2C_CLCD_SendByte2(1, 0b00111);
   I2C_CLCD_SendByte2(1, 0b00011);
   I2C_CLCD_SendByte2(1, 0b00001);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);

   I2C_CLCD_SendByte2(1, 0b11111);   // PutC(3)
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);

   I2C_CLCD_SendByte2(1, 0b00000);   // PutC(4)
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b10000);
   I2C_CLCD_SendByte2(1, 0b11000);
   I2C_CLCD_SendByte2(1, 0b11100);
   I2C_CLCD_SendByte2(1, 0b11110);
   I2C_CLCD_SendByte2(1, 0b11111);

   I2C_CLCD_SendByte2(1, 0b11111);   // PutC(5)
   I2C_CLCD_SendByte2(1, 0b11110);
   I2C_CLCD_SendByte2(1, 0b11100);
   I2C_CLCD_SendByte2(1, 0b11000);
   I2C_CLCD_SendByte2(1, 0b10000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
   I2C_CLCD_SendByte2(1, 0b00000);
}


