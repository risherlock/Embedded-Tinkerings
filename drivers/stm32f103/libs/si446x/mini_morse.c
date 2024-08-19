#include "mini_morse.h"

const char alphameric[] =
{
  // Alphabets
  (char)0b01000010, // A, .-
  (char)0b10000001, // B, -...
  (char)0b10100001, // C, -.-.
  (char)0b10000110, // D, -..
  (char)0b00000100, // E, .
  (char)0b00100001, // F, ..-.
  (char)0b11000110, // G, --.
  (char)0b00000001, // H, ....
  (char)0b00000010, // I, ..
  (char)0b01111001, // J, .---
  (char)0b10100110, // K, -.-
  (char)0b01000001, // L, .-..
  (char)0b11000010, // M, --
  (char)0b10000010, // N, -.
  (char)0b11100110, // O, ---
  (char)0b01100001, // P, .--.
  (char)0b11010001, // Q, --.-
  (char)0b01000110, // R, .-.
  (char)0b00000110, // S, ...
  (char)0b10000100, // T, -
  (char)0b00100110, // U, ..-
  (char)0b00010001, // V, ...-
  (char)0b01100110, // W, .--
  (char)0b10010001, // X, -..-
  (char)0b10110001, // Y, -.--
  (char)0b11000001, // Z, --..

  // Numbers
  (char)0b11111101, // 0, -----
  (char)0b01111101, // 1, .----
  (char)0b00111101, // 2, ..---
  (char)0b00011101, // 3, ...--
  (char)0b00001101, // 4, ....-
  (char)0b00000101, // 5, .....
  (char)0b10000101, // 6, -....
  (char)0b11000101, // 7, --...
  (char)0b11100101, // 8, ---..
  (char)0b11110101, // 9, ----.
};

const struct PunctuationPair
{
  const char letter;
  const char code;
} punctuation[] =
{
  // Punctuations
  {',', (char)0b11001111}, // --..--
  {'.', (char)0b01010111}, // .-.-.-
  {'!', (char)0b10101111}, // -.-.--
  {':', (char)0b11100011}, // ---...
  {';', (char)0b10101011}, // -.-.-.
  {'(', (char)0b10110101}, // -.--.
  {')', (char)0b10110111}, // -.--.-
  {'"', (char)0b01001011}, // .-..-.
  {'@', (char)0b01101011}, // .--.-.
  {'&', (char)0b01000101}, // .-...
  {'?', (char)0b00110011}, // ..--..
  {'-', (char)0b10000111}, // -....-
  {'+', (char)0b01010101}, // .-.-.
  {'/', (char)0b10010101}, // -..-.
  {'=', (char)0b10001101}, // -...-
  {'*', (char)0b10010001}, // -..-
  {'\\', (char)0b01111011}, // .----.
};

void mini_morse_tx(const char* msg, const uint16_t len)
{
  for(uint16_t i = 0; i < len; i++)
  {
    mini_morse_encode_char(msg[i]);
  }
}

char character_to_code(const char x)
{
  char output = 0;
  if (x >= 'A' && x <= 'Z')
  {
    output = alphameric[x - 'A'];
  }
  else if (x >= 'a' && x <= 'z')
  {
    output = alphameric[x - 'a'];
  }
  else if (x >= '0' && x <= '9')
  {
    output = alphameric[x - '0' + 26];
  }
  else
  {
    for (uint8_t i = 0; i < sizeof(punctuation) / 2; i++)
    {
      if (x == punctuation[i].letter)
      {
        output = punctuation[i].code;
      }
    }
  }
  return output;
}

void mini_morse_encode_char(const char x)
{
  if (x == ' ')
  {
    mini_morse_delay(7 * MINI_MORSE_DIT_TIME);
  }
  else
  {
    const char code = character_to_code(x);
    const char length = ((code >> 2) & 1) | (code & 2) | ((code << 2) & 4);
    const char stop = 7 - length + (length > 6);
    for (int i = 7; i > stop; i--)
    {
      if ((code >> i) & 1)
      {
        mini_morse_start_tx(DAH);
        mini_morse_delay(3 * MINI_MORSE_DIT_TIME);
        mini_morse_stop_tx(DAH);
      }
      else
      {
        mini_morse_start_tx(DIT);
        mini_morse_delay(MINI_MORSE_DIT_TIME);
        mini_morse_stop_tx(DIT);
      }
      mini_morse_delay(MINI_MORSE_DIT_TIME);
    }
    mini_morse_delay(2 * MINI_MORSE_DIT_TIME );
  }
}
