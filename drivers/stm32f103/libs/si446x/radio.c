#include "radio.h"
#include "delay.h"
#include "si446x_hal.h"
#include "si446x_ctrl.h"
#include "si446x_defs.h"
#include "radio_config.h"
#include "usart.h"

#define RADIO_PAYLOAD_LENGTH 255
#include <string.h>

uint16_t radio_get_id(void)
{
  uint8_t part_info[4] = {0};
  si446x_ctrl_send_cmd(Si446x_CMD_PART_INFO);
  if(si446x_ctrl_get_response(part_info, sizeof(part_info)))
  {
    return (part_info[2] << 8) + part_info[3];
  }
  return 0;
}

uint8_t radio_init(void)
{
  si446x_hal_init();

  radio_reset();
  si446x_set_properties();

  if(radio_get_id() != Si446x_CONF_ID)
  {
    return 0;
  }

  uint8_t buffer[20] = {0};
  buffer[0] = RF4463_GPIO_NO_CHANGE;
	buffer[1] = RF4463_GPIO_NO_CHANGE;
	buffer[2] = RF4463_GPIO_RX_STATE;
	buffer[3] = RF4463_GPIO_TX_STATE;
	buffer[4] = RF4463_NIRQ_INTERRUPT_SIGNAL;
	buffer[5] = RF4463_GPIO_SPI_DATA_OUT;
	si446x_ctrl_send_cmd_stream(Si446x_CMD_GPIO_PIN_CFG,buffer,6);

  buffer[0] = 98;
  set_properties(Si446x_PROP_GLOBAL_XO_TUNE, buffer, 1);

  buffer[0] = 0x40;
	set_properties(RF4463_PROPERTY_GLOBAL_CONFIG, buffer, 1);

  buffer[0] = 0x08;
	buffer[1] = 0x14;
	buffer[2] = 0x00;
	buffer[3] = 0x0f;
	buffer[4] = RF4463_PREAMBLE_FIRST_1|RF4463_PREAMBLE_LENGTH_BYTES|RF4463_PREAMBLE_STANDARD_1010;
	buffer[5] = 0x00;
	buffer[6] = 0x00;
	buffer[7] = 0x00;
	buffer[8] = 0x00;
	set_properties(Si446x_PROP_PREAMBLE_TX_LENGTH,buffer, 9);

	buffer[0] = 2-1;
	buffer[1] = 0x2d;
	buffer[2] = 0xd4;
	set_properties(Si446x_PROP_SYNC_CONFIG, buffer, 3);

  buffer[0] = RF4463_CRC_SEED_ALL_1S|RF4463_CRC_ITU_T ;
	set_properties(Si446x_PROP_PKT_CRC_CONFIG, buffer, 1);

	buffer[0] = RF4463_CRC_ENDIAN;
	set_properties(Si446x_PROP_PKT_CONFIG1, buffer, 1);

	buffer[0] = RF4463_IN_FIFO | RF4463_DST_FIELD_ENUM_2;
	buffer[1] = RF4463_SRC_FIELD_ENUM_1;
	buffer[2] = 0x00;
	set_properties(Si446x_PROP_PKT_LEN,buffer,3);

  buffer[0] = 0x00;
	buffer[1] = 0x01;
	buffer[2] = RF4463_FIELD_CONFIG_PN_START;
	buffer[3] = RF4463_FIELD_CONFIG_CRC_START|RF4463_FIELD_CONFIG_SEND_CRC|RF4463_FIELD_CONFIG_CHECK_CRC|RF4463_FIELD_CONFIG_CRC_ENABLE;
	buffer[4] = 0x00;
	buffer[5] = 50;
	buffer[6] = RF4463_FIELD_CONFIG_PN_START;
	buffer[7] = RF4463_FIELD_CONFIG_CRC_START|RF4463_FIELD_CONFIG_SEND_CRC|RF4463_FIELD_CONFIG_CHECK_CRC|RF4463_FIELD_CONFIG_CRC_ENABLE;;
	buffer[8]  = 0x00;
	buffer[9] = 0x00;
	buffer[10] = 0x00;
	buffer[11] = 0x00;
	set_properties(Si446x_PROP_PKT_FIELD_1_LENGTH_12_8 ,buffer,12);

  buffer[0] = 0x00;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0x00;
	buffer[4] = 0x00;
	buffer[5] = 0x00;
	buffer[6] = 0x00;
	buffer[7] = 0x00;
	set_properties(Si446x_PROP_PKT_FIELD_4_LENGTH_12_8,buffer,8);

  // radio_set_frequency();
  // radio_configure_packet();
  radio_set_power(127);
  radio_set_state(SLEEP);

  // radio_init_morse();

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
  uint8_t rf_config[] = {0x00,0x00,0x00,0x00,0x00,0x08,0x08};
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
  const uint8_t data=0x03;
	si446x_ctrl_send_cmd_stream(Si446x_CMD_FIFO_INFO,&data,sizeof(data));
  si446x_ctrl_send_cmd_stream(Si446x_CMD_WRITE_TX_FIFO, buffer, len);
  radio_start_tx();

  while (!radio_get_interrupt_status(5, 6)); // PACKET_SENT?
  radio_clear_interrupt_status(2, 6);        // PACKET_SENT_PEND_CLR
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

/**
 * @brief Configure frequency, channel step size, data rate, and
 *  frequency deviation.
 * @cite Si447x PLL Synnthesizer Frequency / VCO_CNT Calculator Rev 0.4
 */
void radio_set_frequency(void)
{
  // Frequency control: Central frequency 433Mhz and channel step size 100kHz
  uint8_t freq_ctrl[] = {0x38, 0xdd, 0xdd, 0xdd, 0x1b, 0x4f};
  set_properties(Si446x_PROP_FREQ_CONTROL_INTE, freq_ctrl, sizeof(freq_ctrl));

  // // Rx voltage controlled oscillator adjust
  // uint8_t vco_adj[] = {0xfe};
  // set_properties(Si446x_PROP_FREQ_CONTROL_VCOCNT_RX_ADJ, vco_adj, sizeof(vco_adj));

  // // Modem data rate: 0.5 kbps
  // uint8_t data_rate[] = {0x00, 0x01, 0xf4};
  // set_properties(Si446x_PROP_MODEM_DATA_RATE_2, data_rate, sizeof(data_rate));

  // // Modem frequency deviation: 1kHz
  // uint8_t freq_div[] = {0x00, 0x00, 0x46};
  // set_properties(Si446x_PROP_MODEM_FREQ_DEV_2, freq_div, sizeof(freq_div));
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
  if(power > 127)
  {
    power = 127;
  }
	uint8_t power_ctrl[]={0x08,power,0x00,0x3d};
  set_properties(Si446x_PROP_PA_MODE, power_ctrl, sizeof(power_ctrl));
}

void radio_init_morse(void)
{
  // Enable GPIO1
  const uint8_t gpio_cmd[] = {0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_GPIO_PIN_CFG, gpio_cmd, sizeof(gpio_cmd));

  // Enable OOK direct Tx
  const uint8_t mode[] = {0b10101001};
  set_properties(Si446x_PROP_MODEM_MOD_TYPE, mode, sizeof(mode));

  // Configure modem
  const uint8_t modem[] = {0xc3, 0x50, 0x01};
  set_properties(Si446x_PROP_MODEM_DATA_RATE_2, modem, sizeof(modem));
  radio_set_state(READY);
}