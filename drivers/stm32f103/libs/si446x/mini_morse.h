/**
 * @brief Morse code encoding with fast search and low memory
 * @author Rishav
 * @date 2023-04-05
**/

/**
 * @note Time spacing
 *  2. Dit              : (1200 / WPM) millis
 *  2. Dah              : 3 dits
 *  3. Between signals  : 1 dit
 *  4. Between letters  : 3 dits
 *  5. Between words    : 7 dits
 *
 * @cite ITU-R M.1677-1 : International Morse Code (2009)
**/

#ifndef _MINI_MORSE_H_
#define _MINI_MORSE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <inttypes.h>

#define MINI_MORSE_WPM 20                         // Words per minute
#define MINI_MORSE_DIT_TIME 1200 / MINI_MORSE_WPM // millis

enum MorseLetter
{
  DIT,
  DAH
};

void mini_morse_tx(const char* msg, const uint16_t len);
void mini_morse_encode_char(const char x);               // Char -> Morse
void mini_morse_start_tx(const enum MorseLetter m); // GPIO pin high
void mini_morse_stop_tx(const enum MorseLetter m);  // GPIO pin low
void mini_morse_delay(const uint16_t delay);   // Delay for a dit

#ifdef __cplusplus
}
#endif
#endif
