#include "util.h"
#include "radio.h"
#include "delay.h"
#include "usart.h"

#include <inttypes.h>

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

  radio_init_gfsk(GFSK_5KBPS_10KHZ);
  radio_set_rx_mode(GFSK_5KBPS_10KHZ);

  while (1)
  {
    uint8_t rx_msg[255] = {0};
    uint8_t len;

    if (radio_rx_gfsk(rx_msg, sizeof(rx_msg), &len))
    {
      usart_tx("Received packet: ");
      usart_tx((char *)rx_msg + 4);
      usart_tx("\n");
    }
  }

  return 0;
}
