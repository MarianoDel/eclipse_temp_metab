/*
 * hard.h
 *
 *  Created on: 28/11/2013
 *      Author: Mariano
 */

#ifndef HARD_H_
#define HARD_H_

#include "stm32f0xx_adc.h"

//--- Board Configuration ---------------------------------------//



//--- End Board Configuration -----------------------------------//


//para GPIO 1 solo bit uso Port bit set/reset register (GPIOx_BSRR) (x=A..G)
//GPIOA pin1
//#define DOOR ((GPIOA->IDR & 0x0002) == 0)
#define DOOR ((GPIOA->IDR & 0x0002) != 0)

//GPIOA pin2
#define LIGHT ((GPIOA->ODR & 0x0004) != 0)
#define LIGHT_ON GPIOA->BSRR = 0x00000004
#define LIGHT_OFF GPIOA->BSRR = 0x00040000

//GPIOA pin3
#define RELAY ((GPIOA->ODR & 0x0008) != 0)
#define RELAY_ON GPIOA->BSRR = 0x00000008
#define RELAY_OFF GPIOA->BSRR = 0x00080000

//GPIOA pin4
#define LED ((GPIOA->ODR & 0x0010) != 0)
#define LED_ON	GPIOA->BSRR = 0x00000010
#define LED_OFF GPIOA->BSRR = 0x00100000



#define CH_IN_POTE ADC_Channel_5
#define CH_IN_TEMP ADC_Channel_0

#define DOOR_ROOF	200
#define DOOR_THRESH	180


//ESTADOS DEL PROGRAMA PRINCIPAL
#define NORMAL	0
#define GO_TO_STOP	1
#define STOPPED	2
#define TO_NEVER	3
#define NEVER	4
#define TO_ALWAYS	5
#define ALWAYS	6

//ESTADOS DEL LED
#define START_BLINKING	0
#define WAIT_TO_OFF	1
#define WAIT_TO_ON	2
#define WAIT_NEW_CYCLE	3


#endif /* HARD_H_ */
