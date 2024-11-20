#include "radio.h"
#include "delay.h"
#include "si446x_hal.h"
#include "si446x_ctrl.h"
#include "si446x_defs.h"
#include "usart.h"

#define RADIO_PAYLOAD_LENGTH 255

#include <string.h>

uint16_t radio_get_id(void)
{
  uint8_t part_info[4] = {0};
  si446x_ctrl_send_cmd(Si446x_CMD_PART_INFO);
  if (si446x_ctrl_get_response(part_info, sizeof(part_info)))
  {
    return (part_info[2] << 8) + part_info[3];
  }
  return 0;
}

uint8_t radio_init(void)
{
  si446x_hal_init();
  delay_ms(10);

  if (radio_get_id() != Si446x_CONF_ID)
  {
    return 0;
  }

  radio_reset();

  const uint8_t cmd[] = {0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_POWER_UP, cmd, sizeof(cmd));
  si446x_ctrl_wait_cts(); // May take longer to set the CTS bit

  set_properties(Si446x_PROP_GLOBAL_XO_TUNE, (const uint8_t[]){98}, 1);

  radio_set_power(127);
  return 1;
}

void radio_reset(void)
{
  si446x_hal_sdn_high();
  delay_ms(100);
  si446x_hal_sdn_low();
  delay_ms(20);
}

void radio_sleep(void)
{
  si446x_hal_sdn_high();
}

RadioState radio_get_state(void)
{
  uint8_t response[3] = {0};
  si446x_ctrl_send_cmd(Si446x_CMD_REQUEST_DEVICE_STATE);
  si446x_ctrl_get_response(response, sizeof(response));
  return response[1] & 0xff;
}

void radio_set_state(RadioState s)
{
  const uint8_t cmd = (uint8_t)s;
  si446x_ctrl_send_cmd_stream(Si446x_CMD_CHANGE_STATE, &cmd, 1);
  si446x_ctrl_wait_cts();
}

void radio_start_tx()
{
  uint8_t rf_config[] = {0x00, 0x30, 0x00, 0x00};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_START_TX, rf_config, sizeof(rf_config));
  // si446x_ctrl_wait_cts();
}

void radio_start_rx()
{
  uint8_t rf_config[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_START_RX, rf_config, sizeof(rf_config));
  // si446x_ctrl_wait_cts();
}

void print_state()
{
  RadioState rs = radio_get_state();

  switch (rs)
  {
  case NO_CHANGE:
    usart_txln("NO_CHANGE");
    break;
  case SLEEP:
    usart_txln("SLEEP");
    break;
  case SPI_ACTIVE:
    usart_txln("SPI_ACTIVE");
    break;
  case READY:
    usart_txln("READY");
    break;
  case TUNE_TX:
    usart_txln("TUNE_TX");
    break;
  case TUNE_RX:
    usart_txln("TUNE_RX");
    break;
  case START_TX:
    usart_txln("START_TX");
    break;
  case START_RX:
    usart_txln("START_RX");
    break;
  default:
    break;
  }
}

#include <string.h>
void radio_tx(const uint8_t *buffer, const uint16_t len)
{
  radio_set_state(SPI_ACTIVE);
  const uint8_t data = 0x03;
  si446x_ctrl_send_cmd_stream(Si446x_CMD_FIFO_INFO, &data, sizeof(data));
  si446x_ctrl_send_cmd_stream(Si446x_CMD_WRITE_TX_FIFO, buffer, len);
  radio_start_tx();

  while (!radio_get_interrupt_status(5, 6)); // PACKET_SENT?
  radio_clear_interrupt_status(2, 6); // PACKET_SENT_PEND_CLR
}

/**
 * @brief Get specific status from the resonse stream of Si446x_CMD_GET_INT_STATUS
 * @note PACKET_SENT status is 6th bit of 5th byte of the response stream.
 *  Therefore to get the PACKET_SENT status: byte = 5 and bit = 6.
 */
uint8_t radio_get_interrupt_status(const uint8_t byte, const uint8_t bit)
{
  uint8_t rsp[9] = {0};
  const uint8_t cmd[3] = {0xff, 0xff, 0xff};

  si446x_ctrl_send_cmd_stream(Si446x_CMD_GET_INT_STATUS, cmd, sizeof(cmd));
  si446x_ctrl_get_response(rsp, sizeof(rsp));

  return (rsp[byte - 1] >> (bit - 1)) & 0x1;
}

/**
 * @brief Change specific pending interrupt.
 * @note PACKET_SENT_PEND_CLR is 6th bit of 2th byte of the response stream.
 *  Therefore to clear the PACKET_SENT_PEND_CLR: byte = 2 and bit = 6.
 */
void radio_clear_interrupt_status(const uint8_t byte, const uint8_t bit)
{
  uint8_t cmd[3] = {0xff, 0xff, 0xff};
  cmd[byte - 1] &= ~(1 << (bit - 1));
  si446x_ctrl_send_cmd_stream(Si446x_CMD_GET_INT_STATUS, cmd, sizeof(cmd));
}

uint8_t set_properties(const uint16_t id, const uint8_t *buff, const uint8_t len)
{
  uint8_t cmd[15] = {0};
  cmd[0] = id >> 8;
  cmd[1] = len;
  cmd[2] = id & 0xff;

  for (uint8_t i = 0; i < len; i++)
  {
    cmd[i + 3] = buff[i];
  }

  si446x_ctrl_send_cmd_stream(Si446x_CMD_SET_PROPERTY, cmd, len + 3);
  return si446x_ctrl_wait_cts();
}

void radio_configure_packet(void)
{
  // Preamble: Length 4
  const uint8_t len = 0x04;
  const uint8_t preamble[] = {len, 0x14, 0x00, 0x00, 0x20 | 0x10 | 0x01};
  set_properties(Si446x_PROP_PREAMBLE_TX_LENGTH, preamble, sizeof(preamble));
  delay_ms(10);

  // Sync word
  const uint8_t sync_words[] = {0x2d, 0xd4};
  uint8_t sync_prop[5] = {0};
  sync_prop[0] = sizeof(sync_words) - 1;
  for (uint8_t i = 0; i < sizeof(sync_words); i++)
  {
    sync_prop[i + 1] = sync_words[i];
  }
  set_properties(Si446x_PROP_SYNC_CONFIG, sync_prop, sizeof(sync_prop));
  delay_ms(10);

  // Packet config: CCIT-16 CRC, 2 fields
  const uint8_t packet[] = {0x05 | 0x80, 0x00, 0x02, 0x01, 0x00, 0x30, 0x30};
  set_properties(Si446x_PROP_PKT_CRC_CONFIG, packet, sizeof(packet));

  // Field 1
  const uint8_t field_1[] = {0x00, 0x01, 0x00, 0x80 | 0x20 | 0x08 | 0x02};
  set_properties(Si446x_PROP_PKT_FIELD_1_LENGTH_12_8, field_1, sizeof(field_1));

  // Field 2
  const uint8_t field_2[] = {0x00, RADIO_PAYLOAD_LENGTH, 0x00, 0x80 | 0x20 | 0x08 | 0x02};
  set_properties(Si446x_PROP_PKT_FIELD_2_LENGTH_12_8, field_2, sizeof(field_2));

  // Clear other fields
  const uint8_t clear[] = {0x00, 0x00, 0x00, 0x00};
  set_properties(Si446x_PROP_PKT_FIELD_3_LENGTH_12_8, clear, sizeof(clear));
  set_properties(Si446x_PROP_PKT_FIELD_4_LENGTH_12_8, clear, sizeof(clear));
  set_properties(Si446x_PROP_PKT_FIELD_5_LENGTH_12_8, clear, sizeof(clear));
}

void radio_set_power(uint8_t power)
{
  if (power > 127)
  {
    power = 127;
  }

  uint8_t power_ctrl[] = {0x08, power, 0x00, 0x3d};
  set_properties(Si446x_PROP_PA_MODE, power_ctrl, sizeof(power_ctrl));
}

void radio_init_morse(void)
{
  const uint8_t modem_config[] = {0xA9, 0x80, 0x1, 0xE0, 0x78, 0x0, 0x22, 0x22};
  set_properties(Si446x_PROP_MODEM_MOD_TYPE, modem_config, sizeof(modem_config));

  const uint8_t freq_config[] = {0x39, 0x9, 0xB4, 0xE8};
  set_properties(Si446x_PROP_FREQ_CONTROL_INTE, freq_config, sizeof(freq_config));

  const uint8_t band_config[] = {0xA};
  set_properties(Si446x_PROP_MODEM_CLKGEN_BAND, band_config, sizeof(band_config));
}

void radio_init_gfsk(void)
{
  /* Modem configuration */

  // const uint8_t ramp_delay[] = {0x01, 0x80, 0x08, 0x03, 0x80, 0x00, 0x00, 0x10};
  // set_properties(Si446x_PROP_MODEM_TX_RAMP_DELAY, ramp_delay, sizeof(ramp_delay));

  const uint8_t modem_config[] = {0x03};
  set_properties(Si446x_PROP_MODEM_MOD_TYPE, modem_config, sizeof(modem_config));

  const uint8_t rate_config[] = {0x00, 0x4E, 0x20};
  set_properties(Si446x_PROP_MODEM_DATA_RATE_2, rate_config, sizeof(rate_config));

  const uint8_t nco_config[] = {0x05, 0xC9, 0xC3, 0x80}; // where?
  set_properties(Si446x_PROP_MODEM_TX_NCO_MODE_3, nco_config, sizeof(nco_config));

  const uint8_t fdev_config[] = {0x00, 0x00, 0x46};
  set_properties(Si446x_PROP_MODEM_FREQ_DEV_2, fdev_config, sizeof(fdev_config));

  const uint8_t band_config[] = {0xA}; // where?
  set_properties(Si446x_PROP_MODEM_CLKGEN_BAND, band_config, sizeof(band_config));

  const uint8_t freq_config[] = {0x39, 0x9, 0xB4, 0xE8};
  set_properties(Si446x_PROP_FREQ_CONTROL_INTE, freq_config, sizeof(freq_config));

  /* Packet configuration */

  // Sync words
  set_properties(Si446x_PROP_SYNC_CONFIG, (const uint8_t[]){0x02-1}, 1);

  const uint8_t crc_config[] = {RF4463_CRC_SEED_ALL_1S | RF4463_CRC_CCITT, 0x01, 0x08, 0xFF, 0xFF, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x30};
  set_properties(Si446x_PROP_PKT_CRC_CONFIG, crc_config, sizeof(crc_config));

  const uint8_t preamble_len[] = {0x04};
  set_properties(Si446x_PROP_PREAMBLE_TX_LENGTH, preamble_len, sizeof(preamble_len));

  const uint8_t preamble_config[] = {0b00110001};
  set_properties(Si446x_PROP_PREAMBLE_CONFIG, preamble_config, sizeof(preamble_config));

  uint8_t pkt_config1[] = {0x00};
  set_properties(Si446x_PROP_PKT_CONFIG1, pkt_config1, sizeof(pkt_config1));

  uint8_t pkt_len[] = {0x02, 0x01, 0x00 };
  set_properties(Si446x_PROP_PKT_LEN, pkt_len, sizeof(pkt_len));

  uint8_t pkt_field1[] = {0x00, 0x01, 0x00,
                          RF4463_FIELD_CONFIG_CRC_START |
                          RF4463_FIELD_CONFIG_SEND_CRC |
                          RF4463_FIELD_CONFIG_CHECK_CRC |
                          RF4463_FIELD_CONFIG_CRC_ENABLE};
  set_properties(Si446x_PROP_PKT_FIELD_1_LENGTH_12_8, pkt_field1, sizeof(pkt_field1));

  uint8_t pkt_field2[] = {0x00, 0x00, 0x00,
                          RF4463_FIELD_CONFIG_CRC_START |
                          RF4463_FIELD_CONFIG_SEND_CRC |
                          RF4463_FIELD_CONFIG_CHECK_CRC |
                          RF4463_FIELD_CONFIG_CRC_ENABLE};
  set_properties(Si446x_PROP_PKT_FIELD_2_LENGTH_12_8, pkt_field2, sizeof(pkt_field2));
}
