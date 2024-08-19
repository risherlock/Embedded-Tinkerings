#include "util.h"
#include "radio.h"
#include "delay.h"
#include "usart.h"
#include "si446x_ctrl.h"
#include "si446x_defs.h"
#include "si446x_hal.h"
#include "mini_morse.h"

// const uint8_t radio_msg[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ,.!:;()\"@&?-+/=*\\";
const uint8_t radio_msg[] = "swwxABCDEFGHIm";

int main()
{
  led_init();
  delay_init();
  usart_init(921600);
  usart_txln("Namastee!");

  while (!radio_init())
  {
    usart_txln("Radio error");
    delay_ms(1000);
  }
  usart_txln("Radio success!");

  // radio_init_morse();
  // mini_morse_tx(radio_msg, sizeof(radio_msg));

  while(0x221b)
  {
    radio_tx(radio_msg, sizeof(radio_msg));
    radio_set_state(SLEEP);
    delay_ms(2000);
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