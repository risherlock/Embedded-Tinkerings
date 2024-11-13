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

  gfsk_tx();

  while (0x221b)
  {
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
void gfsk_tx(void)
{
  uint8_t buffer[6];

  set_properties(Si446x_PROP_GLOBAL_XO_TUNE, (const uint8_t[]){0x4B}, 1);

  set_properties(Si446x_PROP_INT_CTL_ENABLE, (const uint8_t[]){0x05}, 1);
  set_properties(Si446x_PROP_INT_CTL_ENABLE, (const uint8_t[]){0x20}, 1);

  buffer[0] = 0x00;
  buffer[1] = 0x00;
  buffer[2] = 0x00;
  si446x_ctrl_send_cmd_stream(Si446x_CMD_GET_INT_STATUS, buffer, 3);

  set_properties(Si446x_PROP_MODEM_MOD_TYPE, (const uint8_t[]){0x03}, 1);
  set_properties(Si446x_PROP_MODEM_MAP_CONTROL, (const uint8_t[]){0x00}, 1);
  set_properties(Si446x_PROP_MODEM_CLKGEN_BAND, (const uint8_t[]){0x08}, 1);

  set_properties(Si446x_PROP_FREQ_CONTROL_INTE, (const uint8_t[]){0x3C}, 1);
  set_properties(Si446x_PROP_FREQ_CONTROL_FRAC_2, (const uint8_t[]){0x08}, 1);
  set_properties(Si446x_PROP_FREQ_CONTROL_FRAC_1, (const uint8_t[]){0x00}, 1);
  set_properties(Si446x_PROP_FREQ_CONTROL_FRAC_0, (const uint8_t[]){0x00}, 1);
  set_properties(Si446x_PROP_FREQ_CONTROL_W_SIZE, (const uint8_t[]){0x20}, 1);

  set_properties(Si446x_PROP_PA_MODE, (const uint8_t[]){0x08}, 1);
  set_properties(Si446x_PROP_PA_PWR_LVL, (const uint8_t[]){0x7F}, 1);
  set_properties(Si446x_PROP_PA_BIAS_CLKDUTY, (const uint8_t[]){0x00}, 1);
  set_properties(Si446x_PROP_PA_TC, (const uint8_t[]){0x3D}, 1);

  set_properties(Si446x_PROP_MODEM_DATA_RATE_2, (const uint8_t[]){0x00}, 1);
  set_properties(Si446x_PROP_MODEM_DATA_RATE_1, (const uint8_t[]){0x4E}, 1);
  set_properties(Si446x_PROP_MODEM_DATA_RATE_0, (const uint8_t[]){0x20}, 1);
  set_properties(Si446x_PROP_MODEM_TX_NCO_MODE_3, (const uint8_t[]){0x04}, 1);
  set_properties(Si446x_PROP_MODEM_TX_NCO_MODE_2, (const uint8_t[]){0x2D}, 1);
  set_properties(Si446x_PROP_MODEM_TX_NCO_MODE_1, (const uint8_t[]){0xC6}, 1);
  set_properties(Si446x_PROP_MODEM_TX_NCO_MODE_0, (const uint8_t[]){0xC0}, 1);
  set_properties(Si446x_PROP_MODEM_FREQ_DEV_2, (const uint8_t[]){0x00}, 1);
  set_properties(Si446x_PROP_MODEM_FREQ_DEV_1, (const uint8_t[]){0x00}, 1);
  set_properties(Si446x_PROP_MODEM_FREQ_DEV_0, (const uint8_t[]){0x39}, 1);
  set_properties(Si446x_PROP_MODEM_TX_RAMP_DELAY, (const uint8_t[]){0x01}, 1);

  set_properties(Si446x_PROP_PREAMBLE_TX_LENGTH, (const uint8_t[]){0x05}, 1);
  set_properties(Si446x_PROP_PREAMBLE_CONFIG, (const uint8_t[]){0x12}, 1);

  set_properties(Si446x_PROP_SYNC_CONFIG, (const uint8_t[]){0x01}, 1);
  set_properties(Si446x_PROP_SYNC_BITS_31_24, (const uint8_t[]){0xB4}, 1);
  set_properties(Si446x_PROP_SYNC_BITS_23_16, (const uint8_t[]){0x2B}, 1);

  set_properties(Si446x_PROP_PKT_LEN, (const uint8_t[]){0x00}, 1);
  set_properties(Si446x_PROP_PKT_FIELD_1_LENGTH_12_8, (const uint8_t[]){0x00}, 1);
  set_properties(Si446x_PROP_PKT_FIELD_1_LENGTH_7_0, (const uint8_t[]){0x0E}, 1);
  set_properties(Si446x_PROP_PKT_FIELD_1_CONFIG, (const uint8_t[]){0x00}, 1);
  set_properties(Si446x_PROP_PKT_FIELD_1_CRC_CONFIG, (const uint8_t[]){0xA2}, 1);

  const uint8_t payload[] = {0xFF, 0x0F, 0x00, 0x0A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_WRITE_TX_FIFO, payload, sizeof(payload));

  buffer[0] = 0x00;
  buffer[1] = 0x30;
  buffer[2] = 0x00;
  buffer[3] = 0x00;
  si446x_ctrl_send_cmd_stream(Si446x_CMD_START_TX, buffer, 4);
}
