#ifndef _UTIL_H_
#define _UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include <inttypes.h>

void get_str(char *buffer, const int input, const int base);
void led_init(void);

inline void led_on(void)
{
  GPIOC->BRR |= GPIO_BRR_BR13;
}

inline void led_off(void)
{
  GPIOC->BSRR |= GPIO_BSRR_BS13;
}

inline void led_switch(const uint8_t state)
{
  if(state)
  {
    led_on();
  }
  else
  {
    led_off();
  }
}

#ifdef __cplusplus
}
#endif
#endif // util.h
