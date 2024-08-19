/**
 * @brief Hardware abstraction layer
 * @author Rishav
 * @date 2023-06-06 04:55:37 AM +0545
*/

#ifndef _si446x_SPI_H_
#define _si446x_SPI_H_

#include <inttypes.h>
#include "stm32f10x.h"

void si446x_hal_init(void);
void si446x_hal_spi_init(void);
void si446x_hal_sdn_init(void);
void si446x_hal_morse_init(void);

uint8_t si446x_hal_spi_read_byte(const uint8_t cmd);
void si446x_hal_spi_write_byte(const uint8_t cmd);
void si446x_hal_spi_read(uint8_t* buffer, const uint8_t len);
void si446x_hal_spi_write(const uint8_t* buffer, const uint8_t len);

inline void si446x_morse_low(void)
{
  GPIOB->BRR = GPIO_BRR_BR12;
}

inline void si446x_morse_high(void)
{
  GPIOB->BSRR = GPIO_BSRR_BS12;
}

inline void si446x_hal_spi_nsel_low(void)
{
  GPIOA->BRR |= GPIO_BRR_BR4;
}

inline void si446x_hal_spi_nsel_high(void)
{
  GPIOA->BSRR |= GPIO_BSRR_BS4;
}

inline void si446x_hal_sdn_low(void)
{
  GPIOB->BRR |= GPIO_BRR_BR6;
}

inline void si446x_hal_sdn_high(void)
{
  GPIOB->BSRR |= GPIO_BSRR_BS6;
}

#endif // si446x_hal.h
