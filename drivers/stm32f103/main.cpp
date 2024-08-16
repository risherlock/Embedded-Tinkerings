
#include "util.h"
#include "delay.h"
#include "usart.h"
#include "ina219.h"

int main()
{
  led_init();
  delay_init();
  usart_init(115200);

  if(!ina219_init())
  {
    usart_txln("INA219 error!");
    while(1);
  }
  usart_txln("INA219 success!");

  return 0;
}
