#include "util.h"
#include "radio.h"
#include "delay.h"
#include "usart.h"

#include <inttypes.h>

uint8_t rx_msg[100];

int main()
{
  led_init();
  delay_init();
  usart_init(115200);
  usart_txln("Namaste Rx!");

  while (!radio_init())
  {
    usart_txln("Radio error");
    delay_ms(1000);
  }
  usart_txln("Radio success!");

  radio_init_gfsk();
  radio_set_rx_mode();

  while (1)
  {
    // while (radio_available())
    // {
    //   if (radio_rx_gfsk(rx_msg, sizeof(rx_msg)))
    //   {
    //   }
    // }
  }

  return 0;
}
