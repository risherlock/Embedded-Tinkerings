#include "lsm6ds0.h"
#include "stm32f10x.h"
#include "lsm6ds0_regs.h"

#define LSM6DS0_I2C_SLAVE_ADD 0x6B

// I2C macros
#define i2c2_start (I2C2->CR1 |= I2C_CR1_START); while(!(I2C2->SR1 & I2C_SR1_SB))
#define i2c2_stop (I2C2->CR1 |= I2C_CR1_STOP)
#define i2c2_wait_addr while(!(I2C2->SR1 & I2C_SR1_ADDR)); read_clr = I2C2->SR1 | I2C2->SR2
#define i2c2_wait_rxne while(!(I2C2->SR1 & I2C_SR1_RXNE))
#define i2c2_wait_txe while(!(I2C2->SR1 & I2C_SR1_TXE))
#define i2c2_wait_btf while(!(I2C2->SR1 & I2C_SR1_BTF))

static uint16_t read_clr = 0; // Just to reset SR1_ADDR by reading SR1 and SR2.

void i2c1_init(void)
{
  // Clock configurations
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

  // GPIO configuration
  GPIOB->CRH |= GPIO_CRH_CNF10 | GPIO_CRH_MODE10;
  GPIOB->CRH |= GPIO_CRH_CNF11 | GPIO_CRH_MODE11;

  // I2C settings
  I2C2->CR1 = 0;
  I2C2->CR2 = 0;
  I2C2->CR2 = (SystemCoreClock / 1000000);
  I2C2->CCR = (SystemCoreClock / (100000 * 2));
  I2C2->TRISE = ((SystemCoreClock / 1000000) + 1);
  I2C2->CR1 |= I2C_CR1_ACK | I2C_CR1_PE;
}

// ST SAD+W [SAK] SUB [SAK] SR SAD+R [SAK] [DATA] NMAK SP
uint8_t i2c1_read(const uint8_t reg)
{
  uint8_t output = 0;

  i2c2_start;
  I2C2->DR = LSM6DS0_I2C_SLAVE_ADD << 1;
  i2c2_wait_addr;
  I2C2->DR = reg;
  i2c2_wait_txe;
  i2c2_start;
  I2C2->DR = (LSM6DS0_I2C_SLAVE_ADD << 1) | 0x01;
  i2c2_wait_addr;
  I2C2->CR1 &= ~I2C_CR1_ACK;
  i2c2_wait_rxne;
  output = I2C2->DR;
  i2c2_stop;

  return output;
}

void i2c_write(const uint8_t reg, const uint16_t val)
{
  i2c2_start;
  I2C2->DR = LSM6DS0_I2C_SLAVE_ADD << 1;
  i2c2_wait_addr;
  I2C2->DR = reg;
  i2c2_wait_txe;
  I2C2->DR = val;
  i2c2_wait_txe;
  i2c2_wait_btf;
  i2c2_stop;
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
