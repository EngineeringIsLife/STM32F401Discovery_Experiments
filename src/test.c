/*
 * test.c
 *
 *  Created on: Jan 22, 2015
 *      Author: EngineeringIsLife
 */

#include "stm32f4xx.h"
#include "stm32f401_discovery.h"
#include "test.h"
#include "main.h"

void setMagnetLED(void)
{
	uint16_t magvaluex;
	uint16_t magvaluey;

	magvaluex = getMagValue(X);
	magvaluey = getMagValue(Y);

	if (magvaluex & 0x8000) {
		magvaluex = (magvaluex ^ 0xffff) + 1;
		setDirectionLED(UP);
	}
	else setDirectionLED(DOWN);

	if (magvaluey & 0x8000) {
		magvaluey = (magvaluey ^ 0xffff) + 1;
		setDirectionLED(LEFT);
	}
	else setDirectionLED(RIGHT);

	if (magvaluex < 0x60) setDirectionLED(VERTIC_MID);
	if (magvaluey < 0x60) setDirectionLED(HORIZ_MID);

	initMagnet();
}


void setAccLED(void)
{
	uint16_t accvaluex;
	uint16_t accvaluey;

	accvaluex = getAccValue(X);
	accvaluey = getAccValue(Y);

	if (accvaluex & 0x8000) {
		accvaluex = (accvaluex ^ 0xffff) + 1;
		setDirectionLED(UP);
	}
	else setDirectionLED(DOWN);

	if (accvaluey & 0x8000) {
		accvaluey = (accvaluey ^ 0xffff) + 1;
		setDirectionLED(LEFT);
	}
	else setDirectionLED(RIGHT);

	if (accvaluex < 0x019a) setDirectionLED(VERTIC_MID);
	if (accvaluey < 0x019a) setDirectionLED(HORIZ_MID);
}


void setDirectionLED(enum direction dir)
{
	switch(dir)
	{
	case UP:
		STM_EVAL_LEDOn(LED3);
		STM_EVAL_LEDOff(LED6);
		break;
	case DOWN:
		STM_EVAL_LEDOn(LED6);
		STM_EVAL_LEDOff(LED3);
		break;
	case RIGHT:
		STM_EVAL_LEDOn(LED5);
		STM_EVAL_LEDOff(LED4);
		break;
	case LEFT:
		STM_EVAL_LEDOn(LED4);
		STM_EVAL_LEDOff(LED5);
		break;
	case HORIZ_MID:
		STM_EVAL_LEDOff(LED4);
		STM_EVAL_LEDOff(LED5);
		break;
	case VERTIC_MID:
		STM_EVAL_LEDOff(LED3);
		STM_EVAL_LEDOff(LED6);
		break;
	}
}
