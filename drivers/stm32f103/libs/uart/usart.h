#ifndef _USART_H_
#define _USART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

void usart_init(const uint32_t baud_rate);
void usart_tx(const char *msg);
void usart_txln(const char *msg);

#ifdef __cplusplus
}
#endif
#endif // usart.h
