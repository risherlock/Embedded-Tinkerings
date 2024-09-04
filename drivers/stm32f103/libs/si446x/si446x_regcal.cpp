/**
 * @brief Register calculator for Si446x.
 * @cite si446x.pdf
 */

/**
 * @todo
 * 1. outdiv is not same in datasheet and .xlsx calculator.
 */

#include <cmath>
#include <iomanip>
#include <iostream>

// Table 14
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
  const float rf_freq = 915.0; // MHz
  const float freq_xo = 30.0; // MHz
  const float ch_step = 100.0; // kHz
  const float data_rate = 500.0; // kbps
  const float freq_dev = 125.0; // kHz

  const int outdiv = get_outdiv(rf_freq);

  const double n = 0.5f * rf_freq * (double)outdiv / freq_xo;
  const int fc_inte = std::floor(n) - 1;
  const uint32_t fc_frac = (n - fc_inte) * 524288.0f;

  std::cout << "FREQ_CONTROL_INTE = " << std::hex << std::uppercase << "0x" << fc_inte << std::endl;
  std::cout << "FREQ_CONTROL_FRAC2/1/0 = " << std::hex << std::uppercase << "0x" << (fc_frac & 0xFFFFFF) << std::endl;

  return 0;
}
