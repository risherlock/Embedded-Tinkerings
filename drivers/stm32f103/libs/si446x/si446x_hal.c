#include "si446x_hal.h"
#include "si446x_defs.h"
#include "delay.h"

void si446x_hal_init(void)
{
  si446x_hal_spi_init();
  si446x_hal_sdn_init();
  si446x_hal_morse_init();
}

void si446x_hal_morse_init(void)
{
  // GPIO1: PB0
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  GPIOB->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
  GPIOB->CRL |= GPIO_CRL_MODE0_0;
}

void si446x_hal_spi_init(void)
{
  // NSS: PB12
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
  GPIOB->CRH |= GPIO_CRH_MODE12;
  GPIOB->CRH &= ~GPIO_CRH_CNF12;
  GPIOB->BSRR |= GPIO_BSRR_BS12;

  // SCK: PB13
  GPIOB->CRH &= ~GPIO_CRH_CNF13;
  GPIOB->CRH |= GPIO_CRH_MODE13 | GPIO_CRH_CNF13_1;

  // MISO: PB14
  GPIOB->CRH &= ~(GPIO_CRH_MODE14 | GPIO_CRH_CNF14);
  GPIOB->CRH |= GPIO_CRH_CNF14_0;

  // MOSI: PB15
  GPIOB->CRH &= ~GPIO_CRH_CNF15;
  GPIOB->CRH |= (GPIO_CRH_MODE15 | GPIO_CRH_CNF15_1);

  // SPI2
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
  SPI2->CR1 = 0;
  SPI2->CR1 |= SPI_CR1_SSM |
               SPI_CR1_SSI |
               SPI_CR1_BR_2 |
               SPI_CR1_MSTR;

  SPI2->CR1 |= SPI_CR1_SPE;
}

// PA7
void si446x_hal_sdn_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  GPIOA->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_MODE7);
  GPIOA->CRL |= GPIO_CRL_MODE7_1;
}

void si446x_hal_spi_write_byte(const uint8_t cmd)
{
  while (!(SPI2->SR & SPI_SR_TXE));
  SPI2->DR = 0x00ff & cmd;
  while(SPI2->SR & SPI_SR_BSY);
}

uint8_t si446x_hal_spi_read_byte(const uint8_t cmd)
{
  si446x_hal_spi_write_byte(cmd);
  while(!(SPI2->SR & SPI_SR_RXNE));
  uint8_t output = SPI2->DR;
  while(SPI2->SR & SPI_SR_BSY);
  return output;
}

void si446x_hal_spi_read(uint8_t* buffer, const uint8_t len)
{
  for(uint8_t i = 0; i < len; i++)
  {
    buffer[i] = si446x_hal_spi_read_byte(Si446x_CMD_NOP);
  }
}

void si446x_hal_spi_write(const uint8_t* buffer, const uint8_t len)
{
  for(uint8_t i = 0; i < len; i++)
  {
    si446x_hal_spi_write_byte(buffer[i]);
  }
}
