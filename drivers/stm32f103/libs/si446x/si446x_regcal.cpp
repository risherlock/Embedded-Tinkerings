/**
 * @brief Register calculator for Si446x.
 * @cite si446x.pdf
 * @note Some register calculations are reverse engineered from
 * Si446x-RX_HOP-PLL-Calculator.xlsx
 * @date 2024-09-04
 */

/**
 * @todo
 * 1. outdiv is not same in datasheet and .xlsx calculator.
 * 2. Modem data rate is different for rates more than 200 kbps.
 * 3. pa_clock_duty_cycle and pa_output_bias
 */

#include <cmath>
#include <iomanip>
#include <iostream>

#include "si446x_defs.h"

int get_outdiv(float rf_freq)
{
  if ((rf_freq > 1050.0f) || (rf_freq < 142.0))
  {
    return 0;
  }

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
  const int modem_mod_type_reg = RF4463_TX_DIRECT_MODE_TYPE_SYNCHRONOUS |
                                RF4463_TX_DIRECT_MODE_GPIO0 |
                                RF4463_MOD_SOURCE_PACKET_HANDLER |
                                RF4463_MOD_TYPE_2FSK;
  // CW, OOK, FSK, GFSK
  const float rf_freq = 915.0; // MHz
  const float freq_xo = 30.0; // MHz

  // OOK, FSK, GFSK
  const float data_rate = 123.0; // kbps

  // FSK, GFSK
  const float freq_dev = 125.0; // kHz
  const uint8_t ramp_time = RF4463_PA_TC_10_00_US;
  const uint8_t pa_to_mod_delay = RF4463_PA_TC_FSK_MOD_DLY_10_US;

  // PA: CW, OOK, FSK, GFSK
  const uint8_t pa_mode = RF4463_PA_MODE_CLASS_E | RF4463_PA_MODE_4_GROUPS;
  const uint8_t pa_level = 127; // 0-127
  const uint8_t pa_clock_duty_cycle = 0; // Only switched current mode
  const uint8_t pa_output_bias = 0; // 0-63

  // Bit mapping: OOK, FSK, GFSK
  const int modem_map_control_reg = RF4463_MODEM_MAP_CONTROL_ENMANCH
                                    | (~RF4463_MODEM_MAP_CONTROL_ENMANCH & 0)
                                    & ~RF4463_MODEM_MAP_CONTROL_ENINV_RXBIT
                                    & ~RF4463_MODEM_MAP_CONTROL_ENINV_TXBIT
                                    & ~RF4463_MODEM_MAP_CONTROL_ENINV_FD;

  /* Please do not edit anything beyond this point */

  const int outdiv = get_outdiv(rf_freq);
  const double n = 0.5f * rf_freq * (double)outdiv / freq_xo;
  const int fc_inte = std::floor(n) - 1;
  const uint32_t fc_frac = (n - fc_inte) * 524288.0f;
  uint32_t reg_dr = (data_rate > 200) ? data_rate * 10000 :  data_rate * 1000;
  uint32_t reg_fd = 524288.0f * (double)outdiv * freq_dev * 1000 / (2 * freq_xo * 1000000);
  uint8_t reg_pa_tc = 20.0f * 0/ (32.0 - ramp_time) + 0x1D;

  // Frequency configuration
  std::cout << "FREQ_CONTROL_INTE = " << std::hex << std::uppercase << "0x" << fc_inte << std::endl;
  std::cout << "FREQ_CONTROL_FRAC_2/1/0 = " << std::hex << std::uppercase << "0x" << (fc_frac & 0xFFFFFF) << std::endl;

  // Modem configuration
  std::cout << "MODEM_FREQ_DEV_2/1/0 = " << std::hex << std::uppercase << "0x" << ((uint32_t)(reg_fd) & 0xFFFFFF) << std::endl;
  std::cout << "MODEM_MOD_TYPE = " << std::hex << std::uppercase << "0x" << modem_mod_type_reg << std::endl;
  std::cout << "MODEM_MAP_CONTROL = " << std::hex << std::uppercase << "0x" << modem_map_control_reg << std::endl;
  std::cout << "MODEM_DATA_RATE_2/1/0 = " << std::hex << std::uppercase << "0x" << (reg_dr & 0xFFFFFF) << std::endl;

  // Power amplifier
  std::cout << "PA_MODE = " << std::hex << std::uppercase << "0x" << unsigned(pa_mode)<< std::endl;
  std::cout << "PA_PWE_LVL = " << std::hex << std::uppercase << "0x" << unsigned(pa_level)<< std::endl;
  std::cout << "PA_BIAS_CLKDUTY = " << std::hex << std::uppercase << "0x" << 0 << std::endl;
  std::cout << "PA_TC = " << std::hex << std::uppercase << "0x" << (reg_pa_tc | (pa_to_mod_delay << 5)) << std::endl;

  return 0;
}
