#include "util.h"

void get_str(char *buffer, const int input, const int base)
{
  int temp_dataIn;
  temp_dataIn = input;
  int stringLen = 1;

  while ((int)temp_dataIn / base != 0)
  {
    temp_dataIn = (int)temp_dataIn / base;
    stringLen++;
  }
  temp_dataIn = input;

  do
  {
    *(buffer + stringLen - 1) = (temp_dataIn % base) + '0';
    temp_dataIn = (int)temp_dataIn / base;
  } while (stringLen--);
}

void led_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
  GPIOC->CRH |= GPIO_CRH_MODE13_1;
  GPIOC->BSRR = GPIO_BSRR_BS13;
}
