#include "util.h"
#include "radio.h"
#include "delay.h"
#include "usart.h"
#include "si446x_ctrl.h"
#include "si446x_defs.h"
#include "si446x_hal.h"
#include "mini_morse.h"

// const char radio_msg[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ,.!:;()\"@&?-+/=*\\";
const char radio_msg[] = "Namaste!";

void gfsk_tx(void);

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
  // mini_morse_tx(radio_msg, sizeof(radio_msg));

  // radio_init_gfsk();
  // radio_tx(radio_msg, sizeof(radio_msg));

  radio_init_gfsk();

  while (0x221b)
  {
    tx_gfsk();
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

/**
 * @brief Variable-length packet with header (match) bytes
 * @cite an626.pdf page 30.
 * @note Reset and power up is done in radio_init()
 */
void tx_gfsk(void)
{
  const uint8_t payload[] = {11, 0xFF, 0xFF, 0x00, 0x00, 'H', 'e', 'l', 'l', 'o', '!', '\n'};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_WRITE_TX_FIFO, payload, sizeof(payload));

  set_properties(Si446x_PROP_PKT_FIELD_2_LENGTH_7_0, (const uint8_t[]){11}, 1);

  const uint8_t clear_int[] = {0x00, 0x30, 0x00, 0x00};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_START_TX, clear_int, sizeof(clear_int));
}
