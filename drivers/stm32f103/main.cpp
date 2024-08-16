
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

  // while(1)
  // {
  //   if(ina219_get_status())
  //   {
  //     usart_txln("INA219 error!");
  //     while(1);
  //   }

  //   float bus_vol = ina219_get_bus_voltage();
  //   float shunt_vol = ina219_get_shunt_voltage();
  //   float current = ina219_get_current();
  // }

  return 0;
}
