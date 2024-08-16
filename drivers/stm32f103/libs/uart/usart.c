#include "usart.h"
#include "stm32f10x.h"

void usart_init(const uint32_t baud_rate)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
  GPIOA->CRH &= ~GPIO_CRH_CNF9;
  GPIOA->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_CNF9_1;

  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  USART1->BRR = SystemCoreClock / baud_rate;
  USART1->CR1 = 0;
  USART1->CR1 |= USART_CR1_UE | USART_CR1_TE;
}

void usart_tx(const char *msg)
{
  while(*msg)
  {
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = *msg;
    msg++;
  }
}

void usart_txln(const char *msg)
{
  while(*msg)
  {
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = *msg;
    msg++;
  }

  const char nlcr[] = "\r\n";
  for(uint8_t i = 0; i < 2; i++)
  {
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = nlcr[i];
  }
}
