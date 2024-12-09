#include "util.h"
#include "radio.h"
#include "delay.h"
#include "usart.h"
#include "si446x_ctrl.h"
#include "si446x_defs.h"
#include "si446x_hal.h"
#include "mini_morse.h"

#include <inttypes.h>

const uint8_t radio_msg1[] = "01234567890876543210?";
const uint8_t radio_msg2[] = "abcdefghijklmnopqrstuvwxyz!";

void tx_gfsk(const uint8_t *data, const uint8_t n);

int main()
{
  led_init();
  delay_init();
  usart_init(115200);
  usart_txln("Namaste!");

  while (!radio_init())
  {
    usart_txln("Radio error");
    delay_ms(1000);
  }

  usart_txln("Radio success!");
  // radio_init_morse();
  // mini_morse_tx(radio_msg1, sizeof(radio_msg1));

  radio_init_gfsk();
  uint8_t msg_flag = 1;

  while (0x221b)
  {
    delay_ms(2000);

    if (msg_flag)
    {
      radio_tx_gfsk(radio_msg1, sizeof(radio_msg1));
    }
    else
    {
      radio_tx_gfsk(radio_msg2, sizeof(radio_msg2));
    }

    msg_flag = !msg_flag;
  }

  return 0;
}

inline void mini_morse_start_tx(const enum MorseLetter m)
{
  radio_set_state(START_TX);
  si446x_morse_high();
}

inline void mini_morse_stop_tx(const enum MorseLetter m)
{
  si446x_morse_low();
  radio_set_state(READY);
}

inline void mini_morse_delay(const uint16_t delay)
{
  delay_ms(delay);
}
