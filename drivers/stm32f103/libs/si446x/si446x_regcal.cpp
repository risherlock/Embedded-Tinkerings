/**
 * @brief Register calculator for Si446x.
 * @cite si446x.pdf
 * @note Some register calculations are reverse engineered from
 * Si446x-RX_HOP-PLL-Calculator.xlsx
 * @date 2024-09-04
 */

#include <cmath>
#include <iomanip>
#include <iostream>
#include <inttypes.h>

#include "si446x_defs.h"

int get_outdiv(float rf_freq)
{

  if (rf_freq >= 760.0f) return 4;
  if (rf_freq >= 546.0f) return 6;
  if (rf_freq >= 385.0f) return 8;
  if (rf_freq >= 273.0f) return 12;
  if (rf_freq >= 194.0f) return 16;
  if (rf_freq >= 142.0f) return 24;

  return 0;
}

int main()
{
  /* Start configuration */

  // Modem modulation
  const int modem_mod_type_reg = RF4463_TX_DIRECT_MODE_TYPE_ASYNCHRONOUS |
                                 RF4463_TX_DIRECT_MODE_GPIO1 |
                                 RF4463_MOD_SOURCE_DIRECT_MODE |
                                 RF4463_MOD_TYPE_OOK;
  // CW, OOK, FSK, GFSK
  const float rf_freq = 436.6; // MHz
  const float freq_xo = 30.0;  // MHz

  // PA: CW, OOK, FSK, GFSK
  const uint8_t pa_mode = RF4463_PA_MODE_CLASS_E | RF4463_PA_MODE_4_GROUPS;
  const uint8_t pa_level = 127;          // 0-127
  const uint8_t pa_clock_duty_cycle = 0; // Only switched current mode
  const uint8_t pa_output_bias = 0;      // 0-63

  // OOK, FSK, GFSK
  const float data_rate = 123.0; // kbps

  // FSK, GFSK
  const float freq_dev = 125.0; // kHz
  const uint8_t ramp_time = RF4463_PA_TC_10_00_US;
  const uint8_t pa_to_mod_delay = RF4463_PA_TC_FSK_MOD_DLY_10_US;

  // Bit mapping: OOK, FSK, GFSK
  const int modem_map_control_reg = RF4463_MODEM_MAP_CONTROL_ENMANCH | (~RF4463_MODEM_MAP_CONTROL_ENMANCH & 0) & ~RF4463_MODEM_MAP_CONTROL_ENINV_RXBIT & ~RF4463_MODEM_MAP_CONTROL_ENINV_TXBIT & ~RF4463_MODEM_MAP_CONTROL_ENINV_FD;

  // Crystal clock configuration
  const int global_xo_tune = 0x98;
  const int global_clk_config = 0x0;

  // Band
  int band = 0x0A;

  /* Please do not edit anything beyond this point */

  const int outdiv = get_outdiv(rf_freq);
  const double n = 0.5f * rf_freq * (double)outdiv / freq_xo;
  const int fc_inte = std::floor(n) - 1;
  const uint32_t fc_frac = (n - fc_inte) * 524288.0f;
  uint32_t reg_dr = (data_rate > 200) ? data_rate * 10000 : data_rate * 1000;
  uint32_t reg_fd = 524288.0f * (double)outdiv * freq_dev * 1000 / (2 * freq_xo * 1000000);
  uint8_t reg_pa_tc = 20.0f * 0 / (32.0 - ramp_time) + 0x1D;

  // Crystal clock configuration
  std::cout << "GLOBAL_XO_TUNE = " << std::hex << std::uppercase << "0x" << global_xo_tune << std::endl;
  std::cout << "GLOBAL_CLK_CFG = " << std::hex << std::uppercase << "0x" << global_clk_config << std::endl;
  std::cout << std::endl;

  // Frequency configuration
  std::cout << "FREQ_CONTROL_INTE = " << std::hex << std::uppercase << "0x" << fc_inte << std::endl;
  std::cout << "FREQ_CONTROL_FRAC_2/1/0 = " << std::hex << std::uppercase << "0x" << (fc_frac & 0xFFFFFF) << std::endl;
  std::cout << std::endl;

  // Modem configuration
  std::cout << "MODEM_MOD_TYPE = " << std::hex << std::uppercase << "0x" << modem_mod_type_reg << std::endl;
  std::cout << "MODEM_MAP_CONTROL = " << std::hex << std::uppercase << "0x" << modem_map_control_reg << std::endl;
  std::cout << "MODEM_DATA_RATE_2/1/0 = " << std::hex << std::uppercase << "0x" << (reg_dr & 0xFFFFFF) << std::endl;
  std::cout << "MODEM_FREQ_DEV_2/1/0 = " << std::hex << std::uppercase << "0x" << ((reg_fd) & 0xFFFFFF) << std::endl;
  std::cout << std::endl;

  // Power amplifier
  std::cout << "PA_MODE = " << std::hex << std::uppercase << "0x" << unsigned(pa_mode) << std::endl;
  std::cout << "PA_PWE_LVL = " << std::hex << std::uppercase << "0x" << unsigned(pa_level) << std::endl;
  std::cout << "PA_BIAS_CLKDUTY = " << std::hex << std::uppercase << "0x" << 0 << std::endl;
  std::cout << "PA_TC = " << std::hex << std::uppercase << "0x" << (reg_pa_tc | (pa_to_mod_delay << 5)) << std::endl;
  std::cout << std::endl;

  std::cout << "const uint8_t power_amp_config[] = {0x" << std::hex << std::uppercase
            << unsigned(pa_mode) << ", 0x"
            << unsigned(pa_level) << ", 0x"
            << 0 << ", 0x"
            << (reg_pa_tc | (pa_to_mod_delay << 5))
            << "};" << std::endl;
  std::cout << "set_properties(Si446x_PROP_PA_MODE, power_amp_config, sizeof(power_amp_config));" << std::endl;
  std::cout << std::endl;

  std::cout << "const uint8_t modem_config[] = {0x" << std::hex << std::uppercase
            << modem_mod_type_reg << ", 0x"
            << modem_map_control_reg << ", 0x"
            << ((reg_dr >> 16) & 0xFF) << ", 0x"
            << ((reg_dr >> 8) & 0xFF) << ", 0x"
            << (reg_dr & 0xFF) << ", 0x"
            << ((reg_fd >> 16) & 0xFF) << ", 0x"
            << ((reg_fd >> 8) & 0xFF) << ", 0x"
            << (reg_fd & 0xFF)
            << "};" << std::endl;
  std::cout << "set_properties(Si446x_PROP_MODEM_MOD_TYPE, modem_config, sizeof(modem_config));" << std::endl;
  std::cout << std::endl;

  std::cout << "const uint8_t freq_config[] = {0x" << std::hex << std::uppercase
            << fc_inte << ", 0x"
            << ((fc_frac >> 16) & 0xFF) << ", 0x"
            << ((fc_frac >> 8) & 0xFF) << ", 0x"
            << (fc_frac & 0xFF)
            << "};" << std::endl;
  std::cout << "set_properties(Si446x_PROP_FREQ_CONTROL_INTE, freq_config, sizeof(freq_config));" << std::endl;
  std::cout << std::endl;

  std::cout << "const uint8_t band_config[] = {0x" << std::hex << std::uppercase << band << "};" << std::endl;
  std::cout << "set_properties(Si446x_PROP_MODEM_CLKGEN_BAND, band_config, sizeof(band_config));" << std::endl;
  return 0;
}
