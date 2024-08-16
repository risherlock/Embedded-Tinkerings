#ifndef _DELAY_H_
#define _DELAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

void delay_init(void);
void delay_ms(volatile uint32_t ms);

#ifdef __cplusplus
}
#endif
#endif // delay.h