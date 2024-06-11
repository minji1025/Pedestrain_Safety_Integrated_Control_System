/*
 * Escalator.c
 *
 *  Created on: Apr 14, 2024
 *      Author: kccistc
 */

#include "Escalator.h"

uint8_t count = 0;
uint8_t oldcount = 0;
uint16_t counttime = 0;
uint16_t LCDtime = 0;

static ESCALATOR_STATUS_t state = STOP;
static ESCALATOR_STATUS_t oldstate = STOP;
static SPEED_STATUS_t speedState = FAST;
static SPEED_STATUS_t oldespeedState = FAST;

void SysTickcallback() {
	if (counttime > 0)
		counttime--;
	if (LCDtime > 0)
		LCDtime--;
}

void UPLCDinit() {
	I2C_CLCD_Clear2();
	I2C_CLCD_GotoXY2(0, 0);
	I2C_CLCD_PutStr2("               ");
	I2C_CLCD_GotoXY2(0, 1);
	I2C_CLCD_PutStr2("               ");
	I2C_CLCD_GotoXY2(15, 0);
	I2C_CLCD_PutC2(0);
	I2C_CLCD_PutC2(1);
	I2C_CLCD_GotoXY2(15, 1);
	I2C_CLCD_PutC2(2);
	I2C_CLCD_PutC2(3);
}

void UPLCD() {
	if (LCDtime == 0) {
		LCDtime = 700;
		I2C_CLCD_Left2();
	}

}

void DOWNLCDinit()
{
		I2C_CLCD_Clear2();
		I2C_CLCD_GotoXY2(0, 0);
		I2C_CLCD_PutStr2("               ");
		I2C_CLCD_GotoXY2(0, 1);
		I2C_CLCD_PutStr2("               ");
		I2C_CLCD_GotoXY2(0, 0);
		I2C_CLCD_PutC2(1);
		I2C_CLCD_PutC2(4);
		I2C_CLCD_GotoXY2(0, 1);
		I2C_CLCD_PutC2(3);
		I2C_CLCD_PutC2(5);
}
void DOWNLCD() {
	if (LCDtime == 0) {
		LCDtime = 700;
		I2C_CLCD_Right2();
	}
}

void STOPLCDinit()
{
		I2C_CLCD_Clear2();
		I2C_CLCD_GotoXY2(0, 0);
		I2C_CLCD_PutStr2("               ");
		I2C_CLCD_GotoXY2(0, 1);
		I2C_CLCD_PutStr2("               ");
}

void EscalatorSpeedEventCheck() {
	char *msg;

	msg = getMessage();

	if ((strncmp(msg, "Slow", 4) == 0)
			|| (HAL_GPIO_ReadPin(slow_GPIO_Port, slow_Pin) == 0)) {
		speedState = SLOW;
	}
	if ((strncmp(msg, "Fast", 4) == 0)
			|| (HAL_GPIO_ReadPin(fast_GPIO_Port, fast_Pin) == 0)) {
		speedState = FAST;
	}
	if ((strncmp(msg, "Stop", 4) == 0)) {
		state = STOP;
		count = 0;
	}
}

void EscalatorStateCheck() {
	switch (speedState) {
		case FAST:
			EscalatorStateCheck_FAST();
			break;

		case SLOW:
			EscalatorStateCheck_SLOW();
			break;
	}
}
void EscalatorRUN() {
	switch (speedState) {
		case FAST:
			EscalatorRUN_FAST();
			break;

		case SLOW:
			EscalatorRUN_SLOW();
			break;
	}
}

void EscalatorStateCheck_FAST() {
	uint16_t dist1;
	uint16_t dist2;
	dist1 = getDistance();
	dist2 = getDistance2();

	switch (state) {
		case DOWN:
			if ((dist1 > dist2) && (dist2 < 20) && (dist2 > 3)) {
				if (counttime == 0) {
					counttime = 1000;
					count++;
				}
			}
			if ((dist1 < dist2) && (dist1 < 20)) {
				if (counttime == 0) {
					counttime = 1000;
					count--;
					if (count == 0) {
						state = STOP;
					}
				}
			}
			break;
		case UP:
			if ((dist1 < dist2) && (dist1 < 20) && (dist1 > 3)) {
				if (counttime == 0) {
					counttime = 1000;
					count++;
				}
			}
			if ((dist1 > dist2) && (dist2 < 20)) {
				if (counttime == 0) {
					counttime = 1000;
					count--;
					if (count == 0) {
						state = STOP;
					}
				}
			}
			break;

		case STOP:
			if ((dist1 > dist2) && (dist2 < 20) && (dist2 > 3)) {
				if (counttime == 0) {
					counttime = 1000;
					state = DOWN;
					count++;
				}
			} else if ((dist1 < dist2) && (dist1 < 20) && (dist1 > 3)) {
				if (counttime == 0) {
					counttime = 1000;
					state = UP;
					count++;
				}
			}
			break;
	}
}

void EscalatorStateCheck_SLOW() {
	uint16_t dist1;
	uint16_t dist2;
	dist1 = getDistance();
	dist2 = getDistance2();

	switch (state) {
		case DOWN:
			if (counttime == 0) {
				state = STOP;
			}
			break;

		case UP:
			if (counttime == 0) {
				state = STOP;
			}
			break;

		case STOP:
			if ((dist1 > dist2) && (dist2 < 20) && (dist2 > 3)) {
				if (counttime == 0) {
					counttime = 5000;
					state = DOWN;
				}

			} else if ((dist1 < dist2) && (dist1 < 20) && (dist1 > 3)) {
				if (counttime == 0) {
					counttime = 5000;
					state = UP;
				}
			}
			break;
	}
}

void EscalatorRUN_FAST() {

	switch (state) {
		case DOWN:
			if (oldespeedState != speedState || oldstate != state
					|| oldcount != count) {
				goBackward(100);
				I2C_CLCD_GotoXY(0, 1);
				I2C_CLCD_PutStr("               ");
				I2C_CLCD_GotoXY(0, 1);
				I2C_CLCD_PutStr("  FAST  /  DOWN ");
				printf("speed : FAST\n");
				printf("State : DOWN \n");
				printf("Count : %d \n", count);
				oldespeedState = speedState;
				oldstate = state;
				oldcount = count;
				printf("\n");
				DOWNLCDinit();
			}
			DOWNLCD();
			break;
		case UP:
			if (oldespeedState != speedState || oldstate != state
					|| oldcount != count) {
				goForward(100);
				I2C_CLCD_GotoXY(0, 1);
				I2C_CLCD_PutStr("               ");
				I2C_CLCD_GotoXY(0, 1);
				I2C_CLCD_PutStr("  FAST  /  UP  ");
				printf("speed : FAST\n");
				printf("State : UP \n");
				printf("Count : %d \n", count);
				oldespeedState = speedState;
				oldstate = state;
				oldcount = count;
				printf("\n");
				UPLCDinit();
			}
			UPLCD();
			break;
		case STOP:
			if (oldespeedState != speedState || oldstate != state
					|| oldcount != count) {
				stopMove();
				I2C_CLCD_GotoXY(0, 1);
				I2C_CLCD_PutStr("               ");
				I2C_CLCD_GotoXY(0, 1);
				I2C_CLCD_PutStr("  FAST  /  STOP ");
				printf("speed : FAST\n");
				printf("State : STOP \n");
				printf("Count : %d \n", count);
				oldespeedState = speedState;
				oldstate = state;
				oldcount = count;
				printf("\n");
				STOPLCDinit();
			}
			break;
	}
	HAL_Delay(100);
}

void EscalatorRUN_SLOW() {
	switch (state) {
		case DOWN:
			if (oldespeedState != speedState || oldstate != state) {
				goBackward(80);
				I2C_CLCD_GotoXY(0, 1);
				I2C_CLCD_PutStr("               ");
				I2C_CLCD_GotoXY(0, 1);
				I2C_CLCD_PutStr("  SLOW  /  DOWN ");
				printf("speed : SLOW\n");
				printf("State : DOWN \n");
				oldespeedState = speedState;
				oldstate = state;
				printf("\n");
				DOWNLCDinit();
			}
			DOWNLCD();
			break;
		case UP:
			if (oldespeedState != speedState || oldstate != state) {
				goForward(80);
				I2C_CLCD_PutStr("               ");
				I2C_CLCD_GotoXY(0, 1);
				I2C_CLCD_PutStr("  SLOW  /  UP  ");
				printf("speed : SLOW\n");
				printf("State : UP \n");
				oldespeedState = speedState;
				oldstate = state;
				printf("\n");
				UPLCDinit();
			}
			UPLCD();
			break;
		case STOP:
			stopMove();
			if (oldespeedState != speedState || oldstate != state) {
				I2C_CLCD_PutStr("               ");
				I2C_CLCD_GotoXY(0, 1);
				I2C_CLCD_PutStr("  SLOW  /  STOP  ");
				printf("speed : SLOW\n");
				printf("State : STOP \n");
				oldespeedState = speedState;
				oldstate = state;
				printf("\n");
				STOPLCDinit();
			}
			break;
	}
	HAL_Delay(100);
}

