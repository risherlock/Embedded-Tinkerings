#include "usart.h"
#include "ina219.h"
#include "stm32f10x.h"

/**
 * @todo
 *   1. Check math overflow flag for power and current calculations.
 */

// INA219 macros
#define INA219_I2C_SLAVE_ADD 0x40
#define INA219_REG_CONFIGURATION 0x00
#define INA219_REG_SHUNT_VOLTAGE 0x01
#define INA219_REG_BUS_VOLTAGE 0x02
#define INA219_REG_POWER 0x03
#define INA219_REG_CURRENT 0x04
#define INA219_REG_CALIBRATION 0x05

// I2C macros
#define i2c2_start (I2C2->CR1 |= I2C_CR1_START); while(!(I2C2->SR1 & I2C_SR1_SB))
#define i2c2_stop (I2C2->CR1 |= I2C_CR1_STOP)
#define i2c2_wait_addr while(!(I2C2->SR1 & I2C_SR1_ADDR)); read_clr = I2C2->SR1 | I2C2->SR2
#define i2c2_wait_rxne while(!(I2C2->SR1 & I2C_SR1_RXNE))
#define i2c2_wait_txe while(!(I2C2->SR1 & I2C_SR1_TXE))
#define i2c2_wait_btf while(!(I2C2->SR1 & I2C_SR1_BTF))

static uint16_t read_clr = 0; // Just to reset SR1_ADDR by reading SR1 and SR2.
static uint16_t config_val = 0x399F; // Configuration value tracker.
static float current_lsb = 0.0; // Please calibrate for current/power measurements.

void i2c_init(void);
uint16_t i2c_read(const uint8_t reg);
void i2c_write(const uint8_t reg, const uint16_t val);

bool ina219_init(void)
{
  i2c_init();

  return ina219_get_status();
}

// See page 13, ina219.pdf.
bool ina219_calibrate(const float max_current, const float r_shunt)
{
  current_lsb = max_current / 32768.0f;
  const uint16_t calibration_val = (uint16_t)(0.04096 / (current_lsb * r_shunt));
  i2c_write(INA219_REG_CALIBRATION, calibration_val);

  if(i2c_read(INA219_REG_CALIBRATION) == calibration_val)
  {
    return true;
  }

  return false;
}

// ina219_regcal.cpp can be used to generate register value for desired configurations.
bool ina219_configure(const uint16_t val)
{
  i2c_write(INA219_REG_CONFIGURATION, val);

  if(i2c_read(INA219_REG_CONFIGURATION) == val)
  {
    config_val = val;
    return true;
  }

  return false;
}

float ina219_get_bus_voltage(void)
{
  uint16_t reg_val = i2c_read(INA219_REG_BUS_VOLTAGE);
  const float bus_voltage_lsb = 4e-3; // 4mV

  return (reg_val >> 3) * bus_voltage_lsb;
}

float ina219_get_current(void)
{
  uint16_t reg_val = i2c_read(INA219_REG_CURRENT);

  return reg_val * current_lsb;
}

float ina219_get_shunt_voltage(void)
{
  uint16_t reg_val = i2c_read(INA219_REG_SHUNT_VOLTAGE);
  const float shunt_voltage_lsb = 1e-6; // 10uV

  return reg_val * shunt_voltage_lsb;
}

// Watts
float ina219_get_power(void)
{
  uint16_t reg_val = i2c_read(INA219_REG_POWER);

  return reg_val * 20 * current_lsb;
}

// I2C test by reading config register.
bool ina219_get_status(void)
{
  if(i2c_read(INA219_REG_CONFIGURATION) == config_val)
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
 * @cite Figure 276 of rm008.pdf and figure 16 of ina219.pdf.
 */
uint16_t i2c_read(const uint8_t reg)
{
  uint16_t output = 0;

  i2c2_start;
  I2C2->DR = INA219_I2C_SLAVE_ADD << 1;
  i2c2_wait_addr;
  I2C2->DR = reg;
  i2c2_start;
  I2C2->DR = (INA219_I2C_SLAVE_ADD << 1) | 1;
  i2c2_wait_addr;
  i2c2_wait_rxne;
  output = I2C2->DR << 8;
  i2c2_wait_btf;
  output |= I2C2->DR;
  i2c2_stop;

  return output;
}

/**
 * @brief Writes 2 bytes to register.
 * @cite Figure 15 of ina219.pdf.
 */
void i2c_write(const uint8_t reg, const uint16_t val)
{
  i2c2_start;
  I2C2->DR = INA219_I2C_SLAVE_ADD << 1;
  i2c2_wait_addr;
  I2C2->DR = reg;
  i2c2_wait_txe;
  I2C2->DR = val >> 8;
  i2c2_wait_txe;
  I2C2->DR = val & 0xFF;
  i2c2_wait_txe;
  i2c2_stop;
}
