
#include "delay.h"
#include "usart.h"
#include "lsm6ds0.h"

int main()
{
  delay_init();
  usart_init(115200);
  usart_tx("Namaste!\n");

  if(!lsm6ds0_init())
  {
    usart_tx("LSM6DS0 error!\n");
    while(1);
  }
  usart_tx("LSM6DS0 success!\n");

  while(1)
  {
    float a[3], g[3], t;
    lsm6ds0_get_accel(a);
    lsm6ds0_get_gyro(g);
    lsm6ds0_get_temperature(&t);

    usart_tx("ax: ");
    usart_tx_int(a[0]*1000);
    usart_tx(", ay: ");
    usart_tx_int(a[1]*1000);
    usart_tx(", az: ");
    usart_tx_int(a[2]*1000);
    usart_tx(" mg, gx: ");
    usart_tx_int(g[0]*1000);
    usart_tx(", gy: ");
    usart_tx_int(g[1]*1000);
    usart_tx(", gz: ");
    usart_tx_int(g[2]*1000);
    usart_tx(" mdps, t: ");
    usart_tx_int(t);
    usart_tx(" C\n");

    delay_ms(1000);
  }

  return 0;
}
