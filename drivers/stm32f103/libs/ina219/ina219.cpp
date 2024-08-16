#include "ina219.h"
#include "stm32f10x.h"
#include "usart.h"

static uint16_t read_clr = 0;

#define i2c2_start (I2C2->CR1 |= I2C_CR1_START); while(!(I2C2->SR1 & I2C_SR1_SB))
#define i2c2_stop (I2C2->CR1 |= I2C_CR1_STOP)
#define wi2c2_wait_addr while(!(I2C2->SR1 & I2C_SR1_ADDR)); read_clr = I2C2->SR1 | I2C2->SR2
#define i2c2_wait_rxne while(!(I2C2->SR1 & I2C_SR1_RXNE))
#define i2c2_wait_btf while(!(I2C2->SR1 & I2C_SR1_BTF))

bool ina219_init(void)
{
  i2c_init();

  if(i2c_read(INA219_REG_CONFIGURATION) == 0x399F)
  {
    return true;
  }

  return false;
}

// I2C2 -> SCL: PB10, SDA: PB11
void i2c_init(void)
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
  I2C2->CR1 |= I2C_CR1_ACK | I2C_CR1_PE;
  I2C2->CR2 |= (SystemCoreClock / 1000000);
  I2C2->CCR = (SystemCoreClock / (100000 * 2));
  I2C2->TRISE = ((SystemCoreClock / 1000000) + 1);
}

/**
 * @brief Receive two bytes from INA219 in master receiver mode.
 * @cite Figure 276, rm008.pdf
 */
uint16_t i2c_read(const uint8_t reg)
{
  uint16_t output = 0;

  i2c2_start;
  I2C2->DR = INA219_I2C_SLAVE_ADD << 1;
  wi2c2_wait_addr;
  I2C2->DR = reg;
  i2c2_start;
  I2C2->DR = (INA219_I2C_SLAVE_ADD << 1) | 1;
  wi2c2_wait_addr;
  i2c2_wait_rxne;
  output = I2C2->DR << 8;
  i2c2_wait_btf;
  output |= I2C2->DR;
  i2c2_stop;

  return output;
}
