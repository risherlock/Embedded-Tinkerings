#include "lsm6ds0.h"
#include "stm32f10x.h"
#include "lsm6ds0_regs.h"

#define LSM6DS0_I2C_SLAVE_ADD 0x6A

// I2C macros
#define i2c1_start (I2C1->CR1 |= I2C_CR1_START); while(!(I2C1->SR1 & I2C_SR1_SB))
#define i2c1_stop (I2C1->CR1 |= I2C_CR1_STOP)
#define i2c1_wait_addr while(!(I2C1->SR1 & I2C_SR1_ADDR)); read_clr = I2C1->SR1 | I2C1->SR2
#define i2c1_wait_rxne while(!(I2C1->SR1 & I2C_SR1_RXNE))
#define i2c1_wait_txe while(!(I2C1->SR1 & I2C_SR1_TXE))
#define i2c1_wait_btf while(!(I2C1->SR1 & I2C_SR1_BTF))

static uint16_t read_clr = 0; // Just to reset SR1_ADDR by reading SR1 and SR2.

void i2c1_init(void)
{
  // Clock configurations
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  // GPIO configuration: SCL: PB6, SDA: PB7
  GPIOB->CRL |= GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6;
  GPIOB->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7;

  // GPIO configuration: SCL: PB8, SDA: PB9
  // GPIOB->CRH |= (GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8);
  // GPIOB->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9);

  // I2C settings
  I2C1->CR1 = 0;
  I2C1->CR2 = 0;
  I2C1->CR1 |= I2C_CR1_ACK | I2C_CR1_PE;
  I2C1->CR2 |= (SystemCoreClock / 2000000);
  I2C1->CCR = (SystemCoreClock / (400000 *4));
  I2C1->TRISE = ((SystemCoreClock / 2000000) + 1);
}

// ST SAD+W [SAK] SUB [SAK] SR SAD+R [SAK] [DATA] NMAK SP
uint8_t i2c1_read(const uint8_t reg)
{
  uint8_t output = 0;

  i2c1_start;
  I2C1->DR = LSM6DS0_I2C_SLAVE_ADD << 1;
  i2c1_wait_addr;
  I2C1->DR = reg;
  i2c1_wait_addr;
  i2c1_start;
  I2C1->DR = (LSM6DS0_I2C_SLAVE_ADD << 1) | 1;
  i2c1_wait_addr;
  // i2c1_wait_rxne;
  // I2C1->CR1 &= ~I2C_CR1_ACK;
  output = I2C1->DR;
  // i2c1_wait_btf;
  // output |= I2C1->DR;
  i2c1_stop;

  return output;
}

void i2c_write(const uint8_t reg, const uint16_t val)
{
  i2c1_start;
  I2C1->DR = LSM6DS0_I2C_SLAVE_ADD << 1;
  i2c1_wait_addr;
  I2C1->DR = reg;
  i2c1_wait_txe;
  I2C1->DR = val >> 8;
  i2c1_wait_txe;
  I2C1->DR = val & 0xFF;
  i2c1_wait_txe;
  i2c1_stop;
}

bool lsm6ds0_init(void)
{
  i2c1_init();

  if(i2c1_read(LSM6DS0_WHO_AM_I) == LSM6DS0_WHO_AM_I_VAL)
  {
    return true;
  }

  return false;
}
