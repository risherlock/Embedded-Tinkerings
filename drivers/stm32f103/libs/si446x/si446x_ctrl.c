#include "delay.h"
#include "si446x_hal.h"
#include "si446x_ctrl.h"
#include "si446x_defs.h"

#include <string.h>

uint8_t si446x_ctrl_wait_cts(void)
{
  for(uint16_t i=0; i < Si446x_CONF_MAX_CTS_TRIES; i++)
  {
    si446x_hal_spi_nsel_low();
    uint8_t cts = si446x_hal_spi_read_byte(Si446x_CMD_READ_CMD_BUFF);
    si446x_hal_spi_nsel_high();

    if(cts == Si446x_CTS_RESPONSE)
    {
      return 1;
    }
  }
  return 0;
}

uint8_t si446x_ctrl_get_response(uint8_t *buffer, const uint8_t len)
{
  if(si446x_ctrl_wait_cts())
  {
    si446x_hal_spi_nsel_low();
    si446x_hal_spi_read_byte(Si446x_CMD_READ_CMD_BUFF);
    si446x_hal_spi_read(buffer, len);
    si446x_hal_spi_nsel_high();
    return 1;
  }
  return 0;
}

void si446x_ctrl_send_cmd(const uint8_t cmd)
{
  si446x_hal_spi_nsel_low();
  si446x_hal_spi_write_byte(cmd);
  si446x_hal_spi_nsel_high();
  si446x_ctrl_wait_cts();
}

void si446x_ctrl_read_rx_fifo(uint8_t *buffer, const uint8_t len)
{
  si446x_hal_spi_nsel_low();
  si446x_hal_spi_read_byte(Si446x_CMD_READ_RX_FIFO);
  si446x_hal_spi_read(buffer, len);
  si446x_hal_spi_nsel_high();
}

void si446x_ctrl_send_stream(const uint8_t *buffer, const uint8_t len)
{
  si446x_hal_spi_nsel_low();
  si446x_hal_spi_write(buffer, len);
  si446x_hal_spi_nsel_high();
  si446x_ctrl_wait_cts();
}

void si446x_ctrl_send_cmd_stream(const uint8_t cmd, const uint8_t* buffer, const uint8_t len)
{
  si446x_hal_spi_nsel_low();
  si446x_hal_spi_write_byte(cmd);
  si446x_hal_spi_write(buffer, len);
  si446x_hal_spi_nsel_high();
  si446x_ctrl_wait_cts();
}
