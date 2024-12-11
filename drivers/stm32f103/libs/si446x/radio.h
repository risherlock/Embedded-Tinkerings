/**
 * @brief Communication layer
 * @cite Silicon Labs - AN633: Si446x programming guide and sample codes
 * @author Rishav
 * @date 2023-06-06 02:11:23 PM +0545
**/

/**
 * @todo Return status by all functions
 */

#ifndef _Si446x_RADIO_H_
#define _Si446x_RADIO_H_

#include <inttypes.h>
#include <stdbool.h>

typedef enum
{
  NO_CHANGE = 0x00,
  SLEEP = 0x01,
  SPI_ACTIVE = 0x02,
  READY = 0x03,
  TUNE_TX = 0x05,
  TUNE_RX = 0x06,
  START_TX = 0x07,
  START_RX = 0x08
}radio_state_t;

typedef struct
{
	// PH interrupts
	bool filter_match;
	bool filter_miss;
	bool packet_sent;
	bool packet_rx;
	bool crc_error;
	bool tx_fifo_almost_empty;
	bool rx_fifo_almost_full;

	// Modem interrupts
	bool invalid_sync;
	bool rssi_jump;
	bool rssi;
	bool invalid_preamble;
	bool preamble_detect;
	bool sync_detect;

	// Chip interrupts
	bool cal;
	bool fifo_underflow_overflow_error;
	bool state_change;
	bool cmd_error;
	bool chip_ready;
	bool low_batt;
	bool wut;
} radio_interrupts_t;

extern radio_interrupts_t radio_interrupts;

void radio_sleep(void);
uint8_t radio_init(void);

uint8_t radio_available(void);
radio_state_t radio_get_state(void);
void radio_set_state(radio_state_t s);
void radio_set_power(uint8_t power);

void radio_init_gfsk(void);
void radio_init_morse(void);
void radio_set_rx_mode(void);
void radio_set_tx_mode(void);
uint8_t radio_rx_gfsk(uint8_t* data, uint8_t n);
void radio_tx_gfsk(const uint8_t* data, const uint8_t n);

#endif // radio.h
