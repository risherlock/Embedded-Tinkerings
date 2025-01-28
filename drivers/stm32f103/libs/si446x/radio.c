#include "radio.h"
#include "delay.h"
#include "usart.h"
#include "si446x_hal.h"
#include "si446x_ctrl.h"
#include "si446x_defs.h"
#include "radio_config.h"
#include <string.h>

#define RADIO_PAYLOAD_LENGTH 255

void rx_next_fragment(void);

volatile radio_state_t current_radio_state = READY;
radio_interrupts_t radio_interrupts;

typedef struct
{
  int buff_len;
} buff_tracker_t;

static buff_tracker_t buff_tracker;

void radio_reset(void)
{
  si446x_hal_sdn_high();
  delay_ms(150);
  si446x_hal_sdn_low();
  delay_ms(150);
}

void read_interrupts(void)
{
  uint8_t response[8];
  si446x_ctrl_send_cmd(Si446x_CMD_GET_INT_STATUS);
  si446x_ctrl_get_response(response, sizeof(response));

  if(response[0] == 0xFF)
  {
    uint8_t ph_pend = response[4];
    uint8_t modem_pend = response[6];
    uint8_t chip_pend = response[8];

    // PH pending interrupts
    radio_interrupts.filter_match = ((ph_pend & RF4463_INT_STATUS_FILTER_MATCH) >> 7) & 0x01;
    radio_interrupts.filter_miss = ((ph_pend & RF4463_INT_STATUS_FILTER_MISS) >> 6) & 0x01;
    radio_interrupts.packet_sent = ((ph_pend & RF4463_INT_STATUS_PACKET_SENT) >> 5) & 0x01;
    radio_interrupts.packet_rx = ((ph_pend & RF4463_INT_STATUS_PACKET_RX) >> 4) & 0x01;
    radio_interrupts.crc_error = ((ph_pend & RF4463_INT_STATUS_CRC_ERROR) >> 3) & 0x01;
    radio_interrupts.tx_fifo_almost_empty = ((ph_pend & RF4463_INT_STATUS_TX_FIFO_ALMOST_EMPTY) >> 1) & 0x01;
    radio_interrupts.rx_fifo_almost_full = (ph_pend & RF4463_INT_STATUS_RX_FIFO_ALMOST_FULL) & 0x01;

    // Modem interrupts
    radio_interrupts.invalid_sync = ((modem_pend & RF4463_INT_STATUS_INVALID_SYNC) >> 5) & 0x01;
    radio_interrupts.rssi_jump = ((modem_pend & RF4463_INT_STATUS_RSSI_JUMP) >> 4) & 0x01;
    radio_interrupts.rssi = ((modem_pend & RF4463_INT_STATUS_RSSI) >> 3) & 0x01;
    radio_interrupts.invalid_preamble = ((modem_pend & RF4463_INT_STATUS_INVALID_PREAMBLE) >> 2) & 0x01;
    radio_interrupts.preamble_detect = ((modem_pend & RF4463_INT_STATUS_PREAMBLE_DETECT) >> 1) & 0x01;
    radio_interrupts.sync_detect = (modem_pend & RF4463_INT_STATUS_SYNC_DETECT) & 0x01;

    // Chip interrupts
    radio_interrupts.cal = ((chip_pend & RF4463_INT_STATUS_CAL) >> 6) & 0x01;
    radio_interrupts.fifo_underflow_overflow_error = ((chip_pend & RF4463_INT_STATUS_FIFO_UNDERFLOW_OVERFLOW_ERROR) >> 5) & 0x01;
    radio_interrupts.state_change = ((chip_pend & RF4463_INT_STATUS_STATE_CHANGE) >> 4) & 0x01;
    radio_interrupts.cmd_error = ((chip_pend & RF4463_INT_STATUS_CMD_ERROR) >> 3) & 0x01;
    radio_interrupts.chip_ready = ((chip_pend & RF4463_INT_STATUS_CHIP_READY) >> 2) & 0x01;
    radio_interrupts.low_batt = ((chip_pend & RF4463_INT_STATUS_LOW_BATT) >> 1) & 0x01;
    radio_interrupts.wut = (chip_pend & RF4463_INT_STATUS_WUT) & 0x01;
  }
}

void clear_interrupts(void)
{
  const uint8_t clear_int[] = {0x00, 0x00, 0x00};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_GET_INT_STATUS, clear_int, sizeof(clear_int));
  // read_interrupts(); // Update radio_interrupts_t
  memset(&radio_interrupts, 0, sizeof(radio_interrupts));
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

radio_state_t radio_get_state(void)
{
  uint8_t response[3] = {0};
  si446x_ctrl_send_cmd(Si446x_CMD_REQUEST_DEVICE_STATE);
  si446x_ctrl_get_response(response, sizeof(response));
  return response[1] & 0xff;
}

void radio_set_state(radio_state_t s)
{
  const uint8_t cmd = (uint8_t)s;
  si446x_ctrl_send_cmd_stream(Si446x_CMD_CHANGE_STATE, &cmd, 1);
  si446x_ctrl_wait_cts();
}

void print_state()
{
  radio_state_t rs = radio_get_state();

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


// Frequency: 436.6 MHz, Speed: 500 bps, FD: 1 kHz, CRC: CCIT
void radio_init_gfsk(const gfsk_mode_t gfsk)
{
  /* Modem configuration */

  switch (gfsk)
  {
  case GFSK_500BPS_1KHZ:
  {
    const uint8_t rate_config[] = {0x00, 0x4E, 0x20};
    set_properties(Si446x_PROP_MODEM_DATA_RATE_2, rate_config, sizeof(rate_config));

    const uint8_t fdev_config[] = {0x00, 0x00, 0x46};
    set_properties(Si446x_PROP_MODEM_FREQ_DEV_2, fdev_config, sizeof(fdev_config));

    break;
  }

  case GFSK_1KBPS_2KHZ:
  {
    const uint8_t rate_config[] = {0x00, 0x9c, 0x40};
    set_properties(Si446x_PROP_MODEM_DATA_RATE_2, rate_config, sizeof(rate_config));

    const uint8_t fdev_config[] = {0x00, 0x00, 0x8c};
    set_properties(Si446x_PROP_MODEM_FREQ_DEV_2, fdev_config, sizeof(fdev_config));

    break;
  }

  case GFSK_5KBPS_10KHZ:
  {
    const uint8_t rate_config[] = {0x00, 0x9c, 0x40};
    set_properties(Si446x_PROP_MODEM_DATA_RATE_2, rate_config, sizeof(rate_config));

    const uint8_t fdev_config[] = {0x00, 0x00, 0x8c};
    set_properties(Si446x_PROP_MODEM_FREQ_DEV_2, fdev_config, sizeof(fdev_config));

    break;
  }
  }

  const uint8_t modem_config[] = {0x03};
  set_properties(Si446x_PROP_MODEM_MOD_TYPE, modem_config, sizeof(modem_config));

  const uint8_t nco_config[] = {0x05, 0xC9, 0xC3, 0x80}; // where?
  set_properties(Si446x_PROP_MODEM_TX_NCO_MODE_3, nco_config, sizeof(nco_config));

  const uint8_t band_config[] = {0xA}; // where?
  set_properties(Si446x_PROP_MODEM_CLKGEN_BAND, band_config, sizeof(band_config));

  const uint8_t freq_config[] = {0x39, 0x9, 0xB4, 0xE8};
  set_properties(Si446x_PROP_FREQ_CONTROL_INTE, freq_config, sizeof(freq_config));

  /* Packet configuration */

  const uint8_t initint[] = {RF4463_MODEM_INT_STATUS_EN | RF4463_PH_INT_STATUS_EN, 0xff, 0xff, 0x00};
  set_properties(Si446x_PROP_INT_CTL_ENABLE, initint, sizeof(initint));
  clear_interrupts();

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
}

uint8_t tx_buffer[256];

// Figure 12, an633.pdf
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
  while(!radio_interrupts.packet_sent);
  clear_interrupts();

  // Goto sleep mode
  // radio_set_state(SLEEP);
}

// Interrupt handler
void EXTI1_IRQHandler(void)
{
  if (EXTI->PR & EXTI_PR_PR1)
  {
    EXTI->PR |= EXTI_PR_PR1;
    read_interrupts();

    if(radio_interrupts.packet_sent)
    {
      usart_tx("pkt-sent!\n");
    }

    if(radio_interrupts.packet_rx)
    {
	    rx_next_fragment();
      usart_tx("pkt-rx!\n");
    }

    if(radio_interrupts.preamble_detect)
    {
      usart_tx("preamble-detected!\n");
    }

    if(radio_interrupts.sync_detect)
    {
      usart_tx("sync-detected!\n");
    }

    if(radio_interrupts.crc_error)
    {
      usart_tx("crc-error!\n");
    }

    if(radio_interrupts.invalid_sync)
    {
      usart_tx("invalid-sync!\n");
    }

    if(radio_interrupts.invalid_preamble)
    {
      usart_tx("invalid-preamble!\n");
    }

    if (radio_interrupts.rx_fifo_almost_full)
    {
      usart_tx("almost-full!\n");
	    rx_next_fragment();
    }

    // State changes to READY after interrupt
    if (radio_interrupts.packet_rx ||
        radio_interrupts.crc_error ||
        radio_interrupts.invalid_sync ||
        radio_interrupts.invalid_preamble)
    {
      current_radio_state = READY;
    }
  }
}

uint8_t radio_available(void)
{
  // Radio is engaged
  if (current_radio_state == START_RX ||
      current_radio_state == START_TX)
  {
    return 0;
  }

  radio_set_state(START_RX);
  current_radio_state = START_RX;

  return 1;
}

void radio_set_rx_mode(const gfsk_mode_t gfsk)
{
  const uint8_t max_rx_len[] = {50};
  set_properties(Si446x_PROP_PKT_FIELD_2_LENGTH_7_0, max_rx_len, sizeof(max_rx_len));

  const uint8_t reset_fifo[] = {0x02};
  si446x_ctrl_send_cmd_stream(Si446x_CMD_FIFO_INFO, reset_fifo, sizeof(reset_fifo));

  const uint8_t rx_int[3] = {0x03, 0x18, 0x00};
  set_properties(Si446x_PROP_INT_CTL_ENABLE, rx_int, sizeof(rx_int));
  clear_interrupts();

  uint8_t synth[] = {0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03};
  set_properties(Si446x_PROP_SYNTH_PFDCP_CPFF, synth, sizeof(synth));

  switch (gfsk)
  {
  case GFSK_500BPS_1KHZ:
  {
    uint8_t ramp_delay[] = {0x01, 0x80, 0x08, 0x03, 0x80, 0x00};
    set_properties(Si446x_PROP_MODEM_TX_RAMP_DELAY, ramp_delay, sizeof(ramp_delay));

    uint8_t bcr[] = {0x02, 0x71, 0x00, 0xd1, 0xb7, 0x00};
    set_properties(Si446x_PROP_MODEM_BCR_OSR_1, bcr, sizeof(bcr));

    uint8_t mod_decim[] = {0x34, 0x11};
    set_properties(Si446x_PROP_MODEM_DECIMATION_CFG1, mod_decim, sizeof(mod_decim));

    uint8_t mod_raw[] = {0x56, 0x81, 0x00, 0x68};
    set_properties(Si446x_PROP_MODEM_RAW_SEARCH, mod_raw, sizeof(mod_raw));

    uint8_t mod_ook[] = {0x0C, 0xA4, 0x22};
    set_properties(Si446x_PROP_MODEM_OOK_PDTC, mod_ook, sizeof(mod_ook));

    break;
  }

  case GFSK_1KBPS_2KHZ:
  {
    uint8_t ramp_delay[] = {0x01, 0x80, 0x08, 0x03, 0x80, 0x00};
    set_properties(Si446x_PROP_MODEM_TX_RAMP_DELAY, ramp_delay, sizeof(ramp_delay));

    uint8_t bcr[] = {0x03, 0xaa, 0x00, 0x8b, 0xcf, 0x00, 0x46, 0x02, 0xc2, 0x00};
    set_properties(Si446x_PROP_MODEM_BCR_OSR_1, bcr, sizeof(bcr));

    uint8_t mod_decim[] = {0x32, 0x20};
    set_properties(Si446x_PROP_MODEM_DECIMATION_CFG1, mod_decim, sizeof(mod_decim));

    uint8_t mod_raw[] = {0x84, 0x81, 0x00, 0xb1};
    set_properties(Si446x_PROP_MODEM_RAW_SEARCH, mod_raw, sizeof(mod_raw));

    uint8_t mod_ook[] = {0x2b, 0x0c, 0xa4};
    set_properties(Si446x_PROP_MODEM_OOK_PDTC, mod_ook, sizeof(mod_ook));

    uint8_t afc[] = {0x04, 0x23, 0x80, 0x01, 0x5D, 0x35, 0x80};
    set_properties(Si446x_PROP_MODEM_AFC_GEAR, afc, sizeof(afc));

    break;
  }

  case GFSK_5KBPS_10KHZ:
  {
    uint8_t ramp_delay[] = {0x01, 0x80, 0x08, 0x03, 0x80, 0x00};
    set_properties(Si446x_PROP_MODEM_TX_RAMP_DELAY, ramp_delay, sizeof(ramp_delay));

    uint8_t bcr[] = {0x01, 0x77, 0x01, 0x5d, 0x86, 0x00, 0xaf, 0x02, 0xc2, 0x00};
    set_properties(Si446x_PROP_MODEM_BCR_OSR_1, bcr, sizeof(bcr));

    uint8_t mod_decim[] = {0x30, 0x20};
    set_properties(Si446x_PROP_MODEM_DECIMATION_CFG1, mod_decim, sizeof(mod_decim));

    uint8_t mod_raw[] = {0x84, 0x83, 0x00, 0xde, 0x01, 0x00};
    set_properties(Si446x_PROP_MODEM_RAW_SEARCH, mod_raw, sizeof(mod_raw));

    uint8_t mod_ook[] = {0x2a, 0x0c, 0xa4, 0x22};
    set_properties(Si446x_PROP_MODEM_OOK_PDTC, mod_ook, sizeof(mod_ook));

    uint8_t afc[] = {0x04, 0x23, 0x80, 0x0f, 0x15, 0x9a, 0x80};
    set_properties(Si446x_PROP_MODEM_AFC_GEAR, afc, sizeof(afc));

    uint8_t agc[] = {0x11, 0x52, 0x52};
    set_properties(Si446x_PROP_MODEM_AGC_CONTROL, (uint8_t []){0xe0}, 1);
    set_properties(Si446x_PROP_MODEM_AGC_WINDOW_SIZE, agc, sizeof(agc));

    uint8_t fsk_gain[] = {0x80, 0x1a, 0xff, 0xff, 0x00};
    set_properties(Si446x_PROP_MODEM_FSK4_GAIN1, fsk_gain, sizeof(fsk_gain));

    break;
  }
  }

  radio_set_state(START_RX);
  current_radio_state = START_RX;
}

uint8_t rx_buffer[256];

void rx_next_fragment()
{
  // Get length
  uint8_t fifo_info[1] = {0};
  si446x_ctrl_send_cmd(Si446x_CMD_FIFO_INFO);
  si446x_ctrl_get_response(fifo_info, sizeof(fifo_info));
  uint8_t rx_len = fifo_info[0];

  // Check overflow
  if ((rx_len + buff_tracker.buff_len) > 255)
  {
    usart_tx("Overflow\n");
    radio_set_state(SPI_ACTIVE);
    si446x_ctrl_send_cmd_stream(Si446x_CMD_FIFO_INFO, (uint8_t []){0x02}, 1);
    buff_tracker.buff_len = 0;
    return;
  }

  si446x_ctrl_read_rx_fifo(rx_buffer + buff_tracker.buff_len, rx_len);
  buff_tracker.buff_len += fifo_info[0];
}

// Figure 22, an633.pdf
bool radio_rx_gfsk(uint8_t* buff, const uint8_t buff_len, uint8_t* rx_len)
{
  if(!radio_available())
  {
    return false;
  }

  clear_interrupts();

  // Get length
  for (uint8_t i = 0; i < buff_tracker.buff_len; i++)
  {
    buff[i] = rx_buffer[i];
  }

  buff_tracker.buff_len = 0;

  radio_set_state(START_RX);
  current_radio_state = START_RX;

  return true;
}
