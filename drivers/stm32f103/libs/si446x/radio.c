#include "radio.h"
#include "delay.h"
#include "si446x_hal.h"
#include "si446x_ctrl.h"
#include "si446x_defs.h"
#include "usart.h"

#define RADIO_PAYLOAD_LENGTH 255

volatile RadioState current_radio_state = READY;

#include "radio_config.h"

void radio_reset(void)
{
  si446x_hal_sdn_high();
  delay_ms(150);
  si446x_hal_sdn_low();
  delay_ms(150);
}

void configure_wds(void)
{
  static const uint8_t rcda[] = RADIO_CONFIGURATION_DATA_ARRAY;
  uint16_t length_idx = 0;

  while (rcda[length_idx] != 0x00)
  {
    si446x_ctrl_send_stream(&rcda[length_idx + 1], rcda[length_idx]);
    length_idx += rcda[length_idx] + 1;
    delay_ms(20);
  }

  si446x_ctrl_wait_cts();
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

void radio_init_interrupt(void)
{
  // Configure PB1 as input with pull-down
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
  GPIOB->CRL &= ~(GPIO_CRL_MODE1);
  GPIOB->CRL &= ~(GPIO_CRL_CNF1);
  GPIOB->CRL |= GPIO_CRL_CNF1_1;
  GPIOB->ODR |= GPIO_ODR_ODR1;

  // Map PB1 to EXTI1
  AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI1);
  AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PB;

  // EXTI1 as falling edge trigger
  EXTI->IMR |= EXTI_IMR_MR1;
  EXTI->RTSR &= ~(EXTI_RTSR_TR1);
  EXTI->FTSR |= EXTI_FTSR_TR1;

  // Enable EXTI1 interrupt in NVIC
  NVIC_SetPriority(EXTI1_IRQn, 2);
  NVIC_EnableIRQ(EXTI1_IRQn);
}

uint16_t radio_get_id(void)
{
  si446x_ctrl_send_cmd(Si446x_CMD_PART_INFO);

  uint8_t part_info[4] = {0};
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

  // Re-start and start-up sequence
  radio_reset();
  const uint8_t cmd[] = {0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_POWER_UP, cmd, sizeof(cmd));
  si446x_ctrl_wait_cts(); // May take longer to set the CTS bit

  set_properties(Si446x_PROP_GLOBAL_XO_TUNE, (const uint8_t[]){98}, 1);
  radio_set_power(127);
  radio_init_interrupt();

  if (radio_get_id() != Si446x_CONF_ID)
  {
    return 0;
  }

  return 1;
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
  configure_wds();

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

  const uint8_t initint[] = {RF4463_MODEM_INT_STATUS_EN | RF4463_PH_INT_STATUS_EN, 0xff, 0xff, 0x00};
  set_properties(Si446x_PROP_INT_CTL_ENABLE, initint, sizeof(initint));

  const uint8_t clear_int[] = {0x00, 0x00, 0x00};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_GET_INT_STATUS, clear_int, sizeof(clear_int));

  const uint8_t sync_word_len = 2;
  const uint8_t sync_word[] = {sync_word_len-1, 0x2d, 0xd4, 0x00, 0x00};
  set_properties(Si446x_PROP_SYNC_CONFIG, sync_word, sizeof(sync_word));

  const uint8_t crc_config[] = {RF4463_CRC_CCITT | RF4463_CRC_SEED_ALL_1S};
  set_properties(Si446x_PROP_PKT_CRC_CONFIG, crc_config, sizeof(crc_config));

  const uint8_t preamble_len = 4;
  const uint8_t preamble_config[] = {preamble_len, 0x14, 0x00, 0x00,
                                     RF4463_PREAMBLE_FIRST_1 |
                                     RF4463_PREAMBLE_LENGTH_BYTES |
                                     RF4463_PREAMBLE_STANDARD_1010};
  set_properties(Si446x_PROP_PREAMBLE_TX_LENGTH, preamble_config, sizeof(preamble_config));

  const uint8_t pkt_len[] = {0x02, 0x01, 0x00};
  set_properties(Si446x_PROP_PKT_LEN, pkt_len, sizeof(pkt_len));

  // Header field
  uint8_t pkt_field1[] = {0x00, 0x01, 0x00,
                          RF4463_FIELD_CONFIG_CRC_START |
                          RF4463_FIELD_CONFIG_SEND_CRC |
                          RF4463_FIELD_CONFIG_CHECK_CRC |
                          RF4463_FIELD_CONFIG_CRC_ENABLE};
  set_properties(Si446x_PROP_PKT_FIELD_1_LENGTH_12_8, pkt_field1, sizeof(pkt_field1));

  // Payload field
  uint8_t pkt_field2[] = {0x00, 255, 0x00,
                          RF4463_FIELD_CONFIG_CRC_START |
                          RF4463_FIELD_CONFIG_SEND_CRC |
                          RF4463_FIELD_CONFIG_CHECK_CRC |
                          RF4463_FIELD_CONFIG_CRC_ENABLE};
  set_properties(Si446x_PROP_PKT_FIELD_2_LENGTH_12_8, pkt_field2, sizeof(pkt_field2));

  uint8_t pkt_fieldn[] = {0x00, 0x00, 0x00, 0x00};
  set_properties(Si446x_PROP_PKT_FIELD_3_LENGTH_12_8, pkt_fieldn, sizeof(pkt_fieldn));
  set_properties(Si446x_PROP_PKT_FIELD_4_LENGTH_12_8, pkt_fieldn, sizeof(pkt_fieldn));
  set_properties(Si446x_PROP_PKT_FIELD_5_LENGTH_12_8, pkt_fieldn, sizeof(pkt_fieldn));

 /* Rx specific configuration */

}

uint8_t tx_buffer[256];

void radio_tx_gfsk(const uint8_t* data, const uint8_t n)
{
  radio_set_state(SPI_ACTIVE);

  tx_buffer[0] = n + 4;
  tx_buffer[1] = 0xFF;
  tx_buffer[2] = 0xFF;
  tx_buffer[3] = 0x00;
  tx_buffer[4] = 0x00;

  for (uint8_t i = 0; i < n; i++)
  {
    tx_buffer[i+5] = data[i];
  }

  // Set lengths and buffers
  si446x_ctrl_send_cmd_stream(Si446x_CMD_WRITE_TX_FIFO, tx_buffer, 1 + 4 + n);
  set_properties(Si446x_PROP_PKT_FIELD_2_LENGTH_7_0, (const uint8_t[]){4 + n}, 1);

  // Transmit
  radio_set_state(START_TX);
  current_radio_state = START_TX;

  // Wait till Tx complete
  while(current_radio_state != READY);
}

void radio_set_rx_mode(void)
{
  if (current_radio_state != START_RX)
  {
	  uint8_t max_rx_len[] = {255};
	  set_properties(Si446x_PROP_PKT_FIELD_2_LENGTH_7_0, max_rx_len, sizeof(max_rx_len));

    radio_set_state(START_RX);
	  current_radio_state = START_RX;
  }
}

void radio_set_tx_mode(void)
{

}

uint8_t radio_available(void)
{
  if(current_radio_state == START_TX)
  {
    return 0;
  }

  radio_set_state(START_RX);
  current_radio_state = START_RX;
  // usart_txln("radio-available!");

  return 1;
}

uint8_t radio_rx_gfsk(uint8_t* data, uint8_t n)
{
  if(!radio_available())
  {
    return 0;
  }

  const uint8_t clear_int[] = {0x00, 0x00, 0x00};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_GET_INT_STATUS, clear_int, sizeof(clear_int));

  uint8_t fifo_clear[] = {0x02};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_FIFO_INFO, fifo_clear, sizeof(fifo_clear));

  radio_set_state(START_RX);
  current_radio_state = START_RX;

  return 1;
}

// Interrupt handler
void EXTI1_IRQHandler(void)
{
  if (EXTI->PR & EXTI_PR_PR1)
  {
    EXTI->PR |= EXTI_PR_PR1;

    uint8_t status[8];
    si446x_ctrl_send_cmd(Si446x_CMD_GET_INT_STATUS);
    si446x_ctrl_get_response(status, sizeof(status));

    if (status[0] & RF4463_INT_STATUS_MODEM_INT_STATUS)
    {
	    if (status[4] & RF4463_INT_STATUS_INVALID_SYNC)
	    {
        usart_tx("invalid-sync!\n");
        current_radio_state = SPI_ACTIVE;
	    }

      if (status[4] & RF4463_INT_STATUS_INVALID_PREAMBLE)
	    {
        usart_tx("invalid-preamble!\n");
        current_radio_state = SPI_ACTIVE;
	    }
    }

    if(status[0] & RF4463_INT_STATUS_PH_INT_STATUS)
    {
	    if (status[2] &  RF4463_INT_STATUS_CRC_ERROR)
	    {
        usart_tx("crc-error!\n");
        current_radio_state = SPI_ACTIVE;
	    }

      if(status[4] & RF4463_INT_STATUS_PACKET_SENT)
      {
        usart_tx("pkt-sent!\n");
        current_radio_state = SPI_ACTIVE;
      }

      if(status[4] & RF4463_INT_STATUS_PACKET_RX)
      {
        usart_tx("pkt-received!\n");
        current_radio_state = SPI_ACTIVE;
      }
    }
  }
}
