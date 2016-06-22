/**
  ******************************************************************************
  * @file    Template_2/stm32f0x_tim.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   TIM functions.
  ******************************************************************************
  * @attention
  *
  * Use this functions to configure timers.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

//--- VARIABLES EXTERNAS ---//
extern volatile unsigned char timer_1seg;
extern volatile unsigned short timer_led_comm;
extern volatile unsigned short timer_standby;

//--- VARIABLES GLOBALES ---//
volatile unsigned short wait_ms_var = 0;
volatile unsigned short timer_1000 = 0;



//--- FUNCIONES DEL MODULO ---//
void Wait_ms (unsigned short wait)
{
	wait_ms_var = wait;

	while (wait_ms_var);
}

//-------------------------------------------//
// @brief  TIM configure.
// @param  None
// @retval None
//------------------------------------------//
void TIM3_IRQHandler (void)	//1 ms
{

	Usart_Time_1ms ();

	if (timer_1seg)
	{
		if (timer_1000)
			timer_1000--;
		else
		{
			timer_1seg--;
			timer_1000 = 1000;
		}
	}

	if (wait_ms_var)
		wait_ms_var--;

	if (timer_led_comm)
		timer_led_comm--;

	if (timer_standby)
		timer_standby--;

	//bajar flag
	if (TIM3->SR & 0x01)	//bajo el flag
		TIM3->SR = 0x00;
}


void TIM_3_Init (void)
{

	NVIC_InitTypeDef NVIC_InitStructure;

	if (!RCC_TIM3_CLK)
		RCC_TIM3_CLK_ON;

	//Configuracion del timer.
	TIM3->ARR = 100; //1m
	TIM3->CNT = 0;
	TIM3->PSC = 479;
	TIM3->EGR = TIM_EGR_UG;

	// Enable timer ver UDIS
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM14_IRQHandler (void)	//100uS
{

	if (TIM14->SR & 0x01)
		//bajar flag
		TIM14->SR = 0x00;
}


void TIM_14_Init (void)
{

	NVIC_InitTypeDef NVIC_InitStructure;

	if (!RCC_TIM14_CLK)
		RCC_TIM14_CLK_ON;

	//Configuracion del timer.
	TIM14->ARR = 2000; //10m
	TIM14->CNT = 0;
	TIM14->PSC = 479;
	TIM14->EGR = TIM_EGR_UG;

	// Enable timer ver UDIS
	TIM14->DIER |= TIM_DIER_UIE;
	TIM14->CR1 |= TIM_CR1_CEN;

	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM16_IRQHandler (void)	//100uS
{

	if (TIM16->SR & 0x01)
		//bajar flag
		TIM16->SR = 0x00;
}


void TIM_16_Init (void)
{

	NVIC_InitTypeDef NVIC_InitStructure;

	if (!RCC_TIM16_CLK)
		RCC_TIM16_CLK_ON;

	//Configuracion del timer.
	TIM16->ARR = 2000; //10m
	TIM16->CNT = 0;
	TIM16->PSC = 479;
	TIM16->EGR = TIM_EGR_UG;

	// Enable timer ver UDIS
	TIM16->DIER |= TIM_DIER_UIE;
	TIM16->CR1 |= TIM_CR1_CEN;

	NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM17_IRQHandler (void)	//100uS
{

	if (GPIOA_PIN0_OUT)
		GPIOA_PIN0_OFF;
	else
		GPIOA_PIN0_ON;

	if (TIM17->SR & 0x01)
		//bajar flag
		TIM17->SR = 0x00;
}


void TIM_17_Init (void)
{

	NVIC_InitTypeDef NVIC_InitStructure;

	if (!RCC_TIM17_CLK)
		RCC_TIM17_CLK_ON;

	//Configuracion del timer.
	TIM17->ARR = 2000; //10m
	TIM17->CNT = 0;
	TIM17->PSC = 479;
	TIM17->EGR = TIM_EGR_UG;

	// Enable timer ver UDIS
	TIM17->DIER |= TIM_DIER_UIE;
	TIM17->CR1 |= TIM_CR1_CEN;

	NVIC_InitStructure.NVIC_IRQChannel = TIM17_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//--- end of file ---//


