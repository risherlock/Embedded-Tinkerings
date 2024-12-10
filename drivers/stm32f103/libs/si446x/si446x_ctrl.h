/**
 * @brief Si446x commands and configuration layer
 * @author Rishav
 * @cite Silicon Labs - AN633: Si446x programming guide and sample codes
 * @date 2023-06-06 05:27:47 AM +0545
*/

#ifndef _SI4463_CTRL_H_
#define _SI4463_CTRL_H_

#define Si446x_CONF_ID 0x4463
#define Si446x_CONF_MAX_CTS_TRIES 2500

#include <inttypes.h>

void si446x_ctrl_send_cmd(const uint8_t cmd);
void si446x_ctrl_send_stream(const uint8_t *buffer, const uint8_t len);
void si446x_ctrl_send_cmd_stream(const uint8_t cmd, const uint8_t* buffer, const uint8_t len);

uint8_t si446x_ctrl_wait_cts(void);
uint8_t si446x_ctrl_get_response(uint8_t* buffer, const uint8_t len);

#endif // si4463.h
