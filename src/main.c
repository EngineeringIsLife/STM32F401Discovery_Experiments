/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**  Environment : Atollic TrueSTUDIO(R)
**                STMicroelectronics STM32F4xx Standard Peripherals Library
**
**  Distribution: The file is distributed “as is,” without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. This file may only be built (assembled or compiled and linked)
**  using the Atollic TrueSTUDIO(R) product. The use of this file together
**  with other tools than Atollic TrueSTUDIO(R) is not permitted.
**
*****************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32f401_discovery.h"
#include "main.h"
#include "accelerometer.h"

enum direction { RIGHT, LEFT, UP, DOWN, HORIZ_MID, VERTIC_MID };

void initTimerInterrupt(void);
void setDirectionLED(enum direction dir);


void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		//STM_EVAL_LEDToggle(LED3);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

uint16_t accvalue = 0;
void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		//STM_EVAL_LEDToggle(LED5);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

		accvalue = getAccValue(X);
		if ((accvalue & 0x7fff) < 0x019a) setDirectionLED(VERTIC_MID);
		else if (accvalue & 0x8000)	setDirectionLED(UP);
		else						setDirectionLED(DOWN);

		accvalue = getAccValue(Y);
		if ((accvalue & 0x7fff) < 0x019a) setDirectionLED(HORIZ_MID);
		else if (accvalue & 0x8000) setDirectionLED(LEFT);
		else						setDirectionLED(RIGHT);
	}
}


int main(void)
{
  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */

	initLEDsAndButton();
	initTimerInterrupt();
	initAcc();		// Init accelerometer

	while (1)
	{
		asm("nop");
	}
	return 0;
}


/*
 * Callback used by stm32f401_discovery_audio_codec.c.
 * Refer to stm32f401_discovery_audio_codec.h for more info.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm32f401_discovery_audio_codec.c.
 * Refer to stm32f401_discovery_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}

// TODO: Replace by better solution
void delay(uint32_t time)
{
	while (time--);
}

void initLEDsAndButton(void)
{
	  //STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);		// User-Button as GPIO-Pin
	  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);		// User-Button triggers external interrupt at EXTI0
	  STM_EVAL_LEDInit(LED3);
	  STM_EVAL_LEDInit(LED4);
	  STM_EVAL_LEDInit(LED5);
	  STM_EVAL_LEDInit(LED6);
}

void initTimerInterrupt(void)
{
	TIM_TimeBaseInitTypeDef timerInitStruct;

	// Activate TIM3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	// Configure TIM3
	SystemCoreClockUpdate();
	timerInitStruct.TIM_Prescaler = (uint16_t) (SystemCoreClock / 10000);
	timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStruct.TIM_Period = 10 * 10;		// [1/10 ms] Reload-Value
	timerInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &timerInitStruct);

	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);	// Activate interrupt on update-event

	// Configure/Activate TIM3-Interrupt
	NVIC_InitTypeDef NVIC_InitStructure_loc;
	NVIC_InitStructure_loc.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure_loc.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure_loc.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure_loc.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_loc);
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
