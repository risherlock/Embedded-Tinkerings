#include "lsm6ds0.h"
#include "stm32f10x.h"
#include "lsm6ds0_regs.h"

// Please consult datasheet for custom configuration
#define LSM6DS0_ACCEL_g_PER_LSB LSM6DS0_ACCEL_mg_LSB_2G / 1000.0F
#define LSM6DS0_GYRO_DPS_PER_LSB LSM6DS0_GYRO_DPS_DIGIT_2000DPS

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

uint8_t i2c1_read_burst(const uint8_t reg, uint8_t *buff, uint8_t len)
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
  I2C2->CR1 |= I2C_CR1_ACK;

  for(uint8_t n = 0; n < len; n++)
  {
    if(n == len - 1)
    {
      I2C2->CR1 &= ~I2C_CR1_ACK;
    }

    i2c2_wait_rxne;
    buff[n] = I2C2->DR;
  }

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

// CTRL1_GL with ODR_G[3:0] = 0b0000 is power-down.
bool lsm6ds0_init_gyro(void)
{
  uint8_t config_ctrl2_g = 0b01111100; // 833 Hz, +-2000 dps
  i2c_write(LSM6DS0_CTRL2_G, config_ctrl2_g);

  if(i2c1_read(LSM6DS0_CTRL2_G) != config_ctrl2_g)
  {
    return false;
  }

  return true;
}

// CTRL1_XL with ODR_G[3:0] = 0b0000 is power-down.
bool lsm6ds0_init_accel(void)
{
  uint8_t config_ctrl1_xl = 0b01110000; // 833 Hz, +-2g
  i2c_write(LSM6DS0_CTRL1_XL, config_ctrl1_xl);

  if(i2c1_read(LSM6DS0_CTRL1_XL) != config_ctrl1_xl)
  {
    return false;
  }

  return true;
}


bool lsm6ds0_init(void)
{
  i2c1_init();

  if(i2c1_read(LSM6DS0_WHO_AM_I) == LSM6DS0_WHO_AM_I_VAL)
  {
    // Please consult datasheet for custom configuration.
    if(lsm6ds0_init_accel() && lsm6ds0_init_gyro())
    {
      return true;
    }
  }

  return false;
}

// Number of g's i.e. 9.8 m/s^2 = 1.0.
bool lsm6ds0_get_accel(float a[3])
{
  uint8_t data[6];
  i2c1_read_burst(LSM6DS0_OUT_X_L_XL, data, 6);

  a[0] = ((int16_t)(data[1] << 8) | data[0]) * LSM6DS0_ACCEL_g_PER_LSB;
  a[1] = ((int16_t)(data[3] << 8) | data[2]) * LSM6DS0_ACCEL_g_PER_LSB;
  a[2] = ((int16_t)(data[5] << 8) | data[4]) * LSM6DS0_ACCEL_g_PER_LSB;

  return true;
}

// Degrees per second.
bool lsm6ds0_get_gyro(float a[3])
{
  uint8_t data[6];
  i2c1_read_burst(LSM6DS0_OUT_X_L_G, data, 6);

  a[0] = ((int16_t)(data[1] << 8) | data[0]) * LSM6DS0_GYRO_DPS_PER_LSB;
  a[1] = ((int16_t)(data[3] << 8) | data[2]) * LSM6DS0_GYRO_DPS_PER_LSB;
  a[2] = ((int16_t)(data[5] << 8) | data[4]) * LSM6DS0_GYRO_DPS_PER_LSB;

  return true;
}

// Table 4: Temperature sensor characteristics.
bool lsm6ds0_get_temperature(float *t)
{
  uint8_t data[2];
  i2c1_read_burst(LSM6DS0_OUT_TEMP_L, data, 2);

  float t_lsb = data[0] / 256.0f;
  *t = (int8_t)data[1] + t_lsb + 25;

  return true;
}
