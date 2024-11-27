
#include "delay.h"
#include "usart.h"
#include "lsm6ds0.h"

int main()
{
  delay_init();
  usart_init(115200);

  if(!lsm6ds0_init())
  {
    usart_txln("LSM6DS0 error!");
    while(1);
  }
  usart_txln("LSM6DS0 success!");

  while(1)
  {
    delay_ms(1000);
  }

  return 0;
}
