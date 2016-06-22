/**
  ******************************************************************************
  * @file    Template_2/main.h
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Header for gpio module
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F0X_UART_H
#define __STM32F0X_UART_H

//--- Exported types ---//
//--- Exported constants ---//
//--- Exported macro ---//
#define USART1_CLK (RCC->AHBENR & 0x00004000)
#define USART1_CLK_ON RCC->AHBENR |= 0x00004000
#define USART1_CLK_OFF RCC->AHBENR &= ~0x00004000


#define USARTx                           USART1
#define USARTx_CLK                       RCC_APB2Periph_USART1
#define USARTx_APBPERIPHCLOCK            RCC_APB2PeriphClockCmd
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler

#define USARTx_TX_PIN                    GPIO_Pin_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define USARTx_TX_SOURCE                 GPIO_PinSource2
#define USARTx_TX_AF                     GPIO_AF_1

#define USARTx_RX_PIN                    GPIO_Pin_3
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define USARTx_RX_SOURCE                 GPIO_PinSource3
#define USARTx_RX_AF                     GPIO_AF_1

#define USARTx_RX_ENA	USARTx->CR1 &= 0xfffffffb
#define USARTx_RX_DISA	USARTx->CR1 |= 0x04

//--- Exported functions ---//
void USART_Config(void);
void USARTx_receive (void);
unsigned char USARTx_Send(char * send);
void Usart_Time_1ms (void);

#endif //--- End ---//


//--- END OF FILE ---//
