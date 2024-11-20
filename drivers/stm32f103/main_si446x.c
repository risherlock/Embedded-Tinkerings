#include "util.h"
#include "radio.h"
#include "delay.h"
#include "usart.h"
#include "si446x_ctrl.h"
#include "si446x_defs.h"
#include "si446x_hal.h"
#include "mini_morse.h"

const uint8_t radio_msg[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ,.!:;()\"@&?-+/=*\\";
const char cw_msg[] = "Namaste!";

void tx_gfsk(const uint8_t* data, const uint8_t n);

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

  radio_init_morse();
  mini_morse_tx(cw_msg, sizeof(cw_msg));

  radio_init_gfsk();

  while (0x221b)
  {
    delay_ms(2000);
    tx_gfsk(radio_msg, sizeof(radio_msg));
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

uint8_t tx_buffer[256];

void tx_gfsk(const uint8_t* data, const uint8_t n)
{
  tx_buffer[0] = n + 4;
  tx_buffer[1] = 0xFF;
  tx_buffer[2] = 0xFF;
  tx_buffer[3] = 0x00;
  tx_buffer[4] = 0x00;

  for (uint8_t i = 5; i < n + 4; i++)
  {
    tx_buffer[i] = data[i-5];
  }

  si446x_ctrl_send_cmd_stream(Si446x_CMD_WRITE_TX_FIFO, tx_buffer, 1 + 4 + n);
  set_properties(Si446x_PROP_PKT_FIELD_2_LENGTH_7_0, (const uint8_t[]){4 + n}, 1);

  const uint8_t clear_int[] = {0x00, 0x30, 0x00, 0x00};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_START_TX, clear_int, sizeof(clear_int));
}
