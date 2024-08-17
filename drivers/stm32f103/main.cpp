
#include "delay.h"
#include "usart.h"
#include "ina219.h"

#include <cstdio>
int main()
{
  delay_init();
  usart_init(115200);

  if(!ina219_init())
  {
    usart_txln("INA219 error!");
    while(1);
  }
  usart_txln("INA219 success!");

  const float max_current = 1; // Amp
  const float r_shunt = 0.1; // Ohm
  bool status_cal = ina219_calibrate(max_current, r_shunt);

  while(1)
  {
    if(!ina219_get_status())
    {
      usart_txln("INA219 error!");
      while(1);
    }

    float bus_vol = ina219_get_bus_voltage();
    float shunt_vol = ina219_get_shunt_voltage();
    float current = ina219_get_current();

    delay_ms(1000);
  }

  return 0;
}
