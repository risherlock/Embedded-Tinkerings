// INA219 register value generator for Configuration Register.
// 2024-08-17

#include <iostream>
#include <iomanip>

// Bus voltage range: 0b00x0000000000000
const uint16_t config_brng_16v = 0b0000000000000000;
const uint16_t config_brng_32v = 0b0010000000000000;

// Programmable gain adjustment: 0b000xx00000000000
const uint16_t config_pga_pm_40mv  = 0b0000000000000000;
const uint16_t config_pga_pm_80mv  = 0b0000100000000000;
const uint16_t config_pga_pm_160mv = 0b0001000000000000;
const uint16_t config_pga_pm_320mv = 0b0001100000000000;

// Bus voltage ADC: 0b00000xxxx0000000
const uint16_t config_badc_9bit     = 0b0000000000000000;
const uint16_t config_badc_10bit    = 0b0000000010000000;
const uint16_t config_badc_11bit    = 0b0000000100000000;
const uint16_t config_badc_12bit    = 0b0000000110000000;
const uint16_t config_badc_mode_2   = 0b0000010010000000;
const uint16_t config_badc_mode_4   = 0b0000010100000000;
const uint16_t config_badc_mode_8   = 0b0000010110000000;
const uint16_t config_badc_mode_16  = 0b0000011000000000;
const uint16_t config_badc_mode_32  = 0b0000011010000000;
const uint16_t config_badc_mode_64  = 0b0000011100000000;
const uint16_t config_badc_mode_128 = 0b0000011110000000;

// Shunt voltage ADC: 0b000000000xxxx000
const uint16_t config_sadc_9bit     = 0b0000000000000000;
const uint16_t config_sadc_10bit    = 0b0000000000001000;
const uint16_t config_sadc_11bit    = 0b0000000000010000;
const uint16_t config_sadc_12bit    = 0b0000000000011000;
const uint16_t config_sadc_mode_2   = 0b0000000001001000;
const uint16_t config_sadc_mode_4   = 0b0000000001010000;
const uint16_t config_sadc_mode_8   = 0b0000000001011000;
const uint16_t config_sadc_mode_16  = 0b0000000001100000;
const uint16_t config_sadc_mode_32  = 0b0000000001101000;
const uint16_t config_sadc_mode_64  = 0b0000000001110000;
const uint16_t config_sadc_mode_128 = 0b0000000001111000;

// Operating mode: 0b0000000000000xxx
const uint16_t config_mode_power_down           = 0b0000000000000000;
const uint16_t config_mode_shunt_triggered      = 0b0000000000000001;
const uint16_t config_mode_bus_triggered        = 0b0000000000000010;
const uint16_t config_mode_shunt_bus_triggered  = 0b0000000000000011;
const uint16_t config_mode_adc_off              = 0b0000000000000100;
const uint16_t config_mode_shunt_continuous     = 0b0000000000000101;
const uint16_t config_mode_bus_continuous       = 0b0000000000000110;
const uint16_t config_mode_shunt_bus_continuous = 0b0000000000000111;

int main(void)
{
  // Please select desired settings.
  uint16_t config_mode = config_mode_shunt_bus_continuous;
  uint16_t config_sadc = config_sadc_12bit;
  uint16_t config_badc = config_badc_12bit;
  uint16_t config_pga  = config_pga_pm_320mv;
  uint16_t config_brng = config_brng_32v;

  uint16_t configuration_val = config_mode | config_sadc | config_badc | config_pga | config_brng;
  std::cout << "Configuration value = " << std::hex << std::uppercase << "0x" << configuration_val << std::endl;

  return 0;
}
