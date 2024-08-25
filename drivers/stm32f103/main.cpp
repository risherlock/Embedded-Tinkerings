
#include "delay.h"
#include "usart.h"
#include "ina219.h"

int main()
{
  delay_init();
  usart_init(115200);

  if(!ina219_init())
  {
    usart_tx("INA219 error!\n");
    while(1);
  }
  usart_tx("INA219 success!\n");

  const float max_current = 2.5; // Amp
  const float r_shunt = 0.1; // Ohm
  const bool status_cal = ina219_calibrate(max_current, r_shunt);

  // See ina219_regcal.cpp to compute config_val.
  const uint16_t config_val = 0x199F;
  const bool status_config = ina219_configure(config_val);

  if(status_cal && status_config)
  {
    usart_tx("Configuration success!\n");
  }
  else
  {
    usart_tx("Configuration error\n");
    while(1);
  }

  while(1)
  {
    if(!ina219_get_status())
    {
      usart_tx("INA219 error!\n");
      while(1);
    }

    float bus_vol = ina219_get_bus_voltage();
    float shunt_vol = ina219_get_shunt_voltage();
    float current = ina219_get_current();;

    usart_tx("v_bus: ");
    usart_tx_int(int(bus_vol * 1000.0));
    usart_tx(" mV, v_shunt: ");
    usart_tx_int(int(shunt_vol * 1000.0));
    usart_tx(" mV, i_shunt: ");
    usart_tx_int(int(current * 1000.0));
    usart_tx(" mA\n");

    delay_ms(1000);
  }

  return 0;
}
