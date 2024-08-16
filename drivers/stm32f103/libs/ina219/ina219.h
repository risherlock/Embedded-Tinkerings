/**
 * @brief INA219 driver using I2C2 for STM32F103.
 * @cite ina219.pdf, rm008.pdf
 * @date 2024-06-28
 */

#ifndef _INA219_H_
#define _INA219_H_

#include <inttypes.h>

// Configuration and utility functions.
bool ina219_init(void);
bool ina219_get_status(void);
void ina219_save_power(void);
bool ina219_configure(const uint16_t val);
bool ina219_calibrate(const uint16_t val);

// Functions for measurements.
float ina219_get_current(void);
float ina219_get_bus_voltage(void);
float ina219_get_shunt_voltage(void);

#endif // ina219.h
