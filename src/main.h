/**
  ******************************************************************************
  * @file    Template_2/main.h
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
//#include "stm32f0xx_adc.h"
//#include "stm32f0xx_can.h"
//#include "stm32f0xx_cec.h"
//#include "stm32f0xx_comp.h"
//#include "stm32f0xx_crc.h"
//#include "stm32f0xx_crs.h"
//#include "stm32f0xx_dac.h"
//#include "stm32f0xx_dbgmcu.h"
//#include "stm32f0xx_dma.h"
//#include "stm32f0xx_exti.h"
//#include "stm32f0xx_flash.h"
#include "stm32f0xx_gpio.h"
//#include "stm32f0xx_i2c.h"
//#include "stm32f0xx_iwdg.h"
#include "stm32f0xx_misc.h"
//#include "stm32f0xx_pwr.h"
#include "stm32f0xx_rcc.h"
//#include "stm32f0xx_rtc.h"
//#include "stm32f0xx_spi.h"
//#include "stm32f0xx_syscfg.h"
//#include "stm32f0xx_tim.h"
#include "stm32f0xx_usart.h"
//#include "stm32f0xx_wwdg.h"
#include "system_stm32f0xx.h"
#include "stm32f0xx_it.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//--- My includes ---//
#include "stm32f0x_gpio.h"
//#include "stm32f0x_tim.h"
//#include "stm32f0x_uart.h"

//--- Exported types ---//
//--- Exported constants ---//
//--- Exported macro ---//
enum end_states
{
	WORKING = 1,
	END_OK,
	END_ERROR,
	END_TIMEOUT
};
//--- Exported functions ---//

//ESTADOS DEL PROGRAMA PRINCIPAL
#define STAND_BY	0
#define TX_LCD		1
#define TX_LCD2		2
#define TX_SERIE	3
#define TX_SERIE2	4
#define RX_SERIE	5
#define TEMP_SENSE	6
#define TX_SERIE_NC		7
#define TX_SERIE2_NC	8
#define CONNECT	9


//ESTADOS DEL LED DE COMUNICACION
//#define NO_COMM		0
//#define TX_NO_COMM	1
//#define TX_COMM			2
//#define COMM		3

//FUNCIONES DEL MAIN QUE USAN OTROS MODULOS???
void TimingDelay_Decrement(void);

#endif //--- End ---//


//--- END OF FILE ---//
