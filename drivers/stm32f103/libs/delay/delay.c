#include "delay.h"
#include "stm32f10x.h"

static volatile uint32_t timekeeper = 0;

void delay_init(void)
{
  SysTick_Config(SystemCoreClock / 1000);
  __enable_irq();
}

void delay_ms(volatile uint32_t ms)
{
  timekeeper = 0;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
  while (timekeeper < ms);
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler()
{
  timekeeper++;
}
