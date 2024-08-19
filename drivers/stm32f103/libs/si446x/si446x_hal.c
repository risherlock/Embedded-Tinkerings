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
  // GPIO1: BP12
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  GPIOB->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
  GPIOB->CRH |= GPIO_CRH_MODE12_0;
}

void si446x_hal_spi_init(void)
{
  // NSS
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
  GPIOA->CRL |= GPIO_CRL_MODE4;
  GPIOA->CRL &= ~GPIO_CRL_CNF4;
  GPIOA->BSRR |= GPIO_BSRR_BS4;

  // SCK
  GPIOA->CRL &= ~GPIO_CRL_CNF5;
  GPIOA->CRL |= GPIO_CRL_MODE5 | GPIO_CRL_CNF5_1;

  // MISO
  GPIOA->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
  GPIOA->CRL |= GPIO_CRL_CNF6_0;

  // MOSI
  GPIOA->CRL &= ~GPIO_CRL_CNF7;
  GPIOA->CRL |= (GPIO_CRL_MODE7 | GPIO_CRL_CNF7_1);

  // SPI
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  SPI1->CR1 = 0;
  SPI1->CR1 |= SPI_CR1_SSM |
               SPI_CR1_SSI |
               SPI_CR1_BR_2 |
               SPI_CR1_MSTR;

  SPI1->CR1 |= SPI_CR1_SPE;
}

void si446x_hal_sdn_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  GPIOB->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_MODE6);
  GPIOB->CRL |= GPIO_CRL_MODE6_1;
}

void si446x_hal_spi_write_byte(const uint8_t cmd)
{
  while (!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = 0x00ff & cmd;
  while((SPI1->SR & SPI_SR_BSY));
}

uint8_t si446x_hal_spi_read_byte(const uint8_t cmd)
{
  si446x_hal_spi_write_byte(cmd);
  while(!(SPI1->SR & SPI_SR_RXNE));
  uint8_t output = SPI1->DR;
  while((SPI1->SR & SPI_SR_BSY));
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
