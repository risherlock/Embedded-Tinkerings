// 2024-06-28

#ifndef _INA219_H_
#define _INA219_H_

#include <inttypes.h>

#define INA219_I2C_SLAVE_ADD 0x40

// INA219 registers
#define INA219_REG_CONFIGURATION 0x00
#define INA219_REG_SHUNT_VOLTAGE 0x01
#define INA219_REG_BUS_VOLTAGE 0x02
#define INA219_REG_POWER 0x03
#define INA219_REG_CURRENT 0x04
#define INA219_REG_CALIBRATION 0x05

bool ina219_init(void);
void ina219_save_power(void);
bool ina219_configure(void);

bool ina219_get_voltage(float *voltage);
bool ina219_get_current(float *current);
bool ina219_get_shunt_voltage(float *voltage);

void i2c_init(void);
uint16_t i2c_read(const uint8_t reg);
bool i2c_write(const uint8_t add, const uint16_t data);

#endif // ina219.h
