/**
 * @brief Communication layer
 * @cite Silicon Labs - AN633: Si446x programming guide and sample codes
 * @author Rishav
 * @date 2023-06-06 02:11:23 PM +0545
**/

#ifndef _Si446x_RADIO_H_
#define _Si446x_RADIO_H_

#include <inttypes.h>

typedef enum RadioStateEnum
{
  NO_CHANGE = 0x00,
  SLEEP = 0x01,
  SPI_ACTIVE = 0x02,
  READY = 0x03,
  TUNE_TX = 0x05,
  TUNE_RX = 0x06,
  START_TX = 0x07,
  START_RX = 0x08
}RadioState;

uint8_t radio_init(void);
void radio_reset(void);
void radio_sleep(void);

RadioState radio_get_state(void);
void radio_set_state(RadioState s);
uint8_t radio_get_interrupt_status(const uint8_t byte, const uint8_t bit);
void radio_clear_interrupt_status(const uint8_t byte, const uint8_t bit);

void radio_set_frequency(void);
void radio_set_power(uint8_t power);
void radio_configure_packet(void);
uint8_t set_properties(const uint16_t id, const uint8_t *buff, const uint8_t len);

void radio_init_morse(void);
void radio_init_gfsk(void);
void radio_tx_gfsk(const uint8_t* data, const uint8_t n);

#endif // radio.h
