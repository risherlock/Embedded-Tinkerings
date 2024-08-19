/**
 * @brief API descriptions to control and configure Si446x
 * @author Rishav
 * @cite Silicon Labs - AN625: Si446x API descriptions
 * @date 2023-06-06
**/

#ifndef _si446x_ctrl_H_
#define _si446x_ctrl_H_

/* Expected responses */
#define Si446x_CTS_RESPONSE 0xFF

/* Control commands */
#define Si446x_CMD_NOP 0x00
#define Si446x_CMD_PART_INFO 0x01
#define Si446x_CMD_POWER_UP 0x02
#define Si446x_CMD_FUNC_INFO 0x10
#define Si446x_CMD_SET_PROPERTY 0x11
#define Si446x_CMD_GET_PROPERTY 0x12
#define Si446x_CMD_GPIO_PIN_CFG 0x13
#define Si446x_CMD_GET_ADC_READING 0x14
#define Si446x_CMD_FIFO_INFO 0x15
#define Si446x_CMD_PACKET_INFO 0x16
#define Si446x_CMD_IRCAL 0x17
#define Si446x_CMD_PROTOCOL_CFG 0x18
#define Si446x_CMD_GET_INT_STATUS 0x20
#define Si446x_CMD_GET_PH_STATUS 0x21
#define Si446x_CMD_GET_MODEM_STATUS 0x22
#define Si446x_CMD_GET_CHIP_STATUS 0x23
#define Si446x_CMD_START_TX 0x31
#define Si446x_CMD_START_RX 0x32
#define Si446x_CMD_REQUEST_DEVICE_STATE 0x33
#define Si446x_CMD_CHANGE_STATE 0x34
#define Si446x_CMD_READ_CMD_BUFF 0x44
#define Si446x_CMD_FRR_A_READ 0x50
#define Si446x_CMD_FRR_B_READ 0x51
#define Si446x_CMD_FRR_C_READ 0x53
#define Si446x_CMD_FRR_D_READ 0x57
#define Si446x_CMD_WRITE_TX_FIFO 0x66
#define Si446x_CMD_READ_RX_FIFO 0x77
#define Si446x_CMD_RX_HOP 0x36

/* Configuration settings */

// Clock (2)
#define Si446x_PROP_GLOBAL_XO_TUNE 0x0000
#define Si446x_PROP_GLOBAL_CLK_CFG 0x0001
#define RF4463_PROPERTY_GLOBAL_LOW_BATT_THRESH           0x0002
#define RF4463_PROPERTY_GLOBAL_CONFIG                    0x0003

// Wake-up timer (5)
#define Si446x_PROP_GLOBAL_WUT_CONFIG 0x0004
#define Si446x_PROP_GLOBAL_WUT_M_15_8 0x0005
#define Si446x_PROP_GLOBAL_WUT_M_7_0 0x0006
#define Si446x_PROP_GLOBAL_WUT_R 0x0007
#define Si446x_PROP_GLOBAL_WUT_LD 0x0008

// Interrupt (4)
#define Si446x_PROP_INT_CTL_ENABLE 0x0100
#define Si446x_PROP_INT_CTL_PH_ENABLE 0x0101
#define Si446x_PROP_INT_CTL_MODEM_ENABLE 0x0102
#define Si446x_PROP_INT_CTL_CHIP_ENABLE 0x0103

// Fast response registers (4)
#define Si446x_PROP_FRR_CTL_A_MODE 0x0200
#define Si446x_PROP_FRR_CTL_B_MODE 0x0201
#define Si446x_PROP_FRR_CTL_C_MODE 0x0202
#define Si446x_PROP_FRR_CTL_D_MODE 0x0203

// Preamble (9)
#define Si446x_PROP_PREAMBLE_TX_LENGTH 0x1000
#define Si446x_PROP_PREAMBLE_CONFIG_STD_1 0x1001
#define Si446x_PROP_PREAMBLE_CONFIG_NSTD 0x1002
#define Si446x_PROP_PREAMBLE_CONFIG_STD_2 0x1003
#define Si446x_PROP_PREAMBLE_CONFIG 0x1004
#define Si446x_PROP_PREAMBLE_PATTERN_31_24 0x1005
#define Si446x_PROP_PREAMBLE_PATTERN_23_16 0x1006
#define Si446x_PROP_PREAMBLE_PATTERN_15_8 0x1007
#define Si446x_PROP_PREAMBLE_PATTERN_7_0 0x1008

// Sync word (5)
#define Si446x_PROP_SYNC_CONFIG 0x1100
#define Si446x_PROP_SYNC_BITS_31_24 0x1101
#define Si446x_PROP_SYNC_BITS_23_16 0x1102
#define Si446x_PROP_SYNC_BITS_15_8 0x1103
#define Si446x_PROP_SYNC_BITS_7_0 0x1104

// Packet config (7)
#define Si446x_PROP_PKT_CRC_CONFIG 0x1200
#define Si446x_PROP_PKT_CONFIG1 0x1206
#define Si446x_PROP_PKT_LEN 0x1208
#define Si446x_PROP_PKT_LEN_FIELD_SOURCE 0x1209
#define Si446x_PROP_PKT_LEN_ADJUST 0x120A
#define Si446x_PROP_PKT_TX_THRESHOLD 0x120B
#define Si446x_PROP_PKT_RX_THRESHOLD 0x120C

// Field 1 (4)
#define Si446x_PROP_PKT_FIELD_1_LENGTH_12_8 0x120D
#define Si446x_PROP_PKT_FIELD_1_LENGTH_7_0 0x120E
#define Si446x_PROP_PKT_FIELD_1_CONFIG 0x120F
#define Si446x_PROP_PKT_FIELD_1_CRC_CONFIG 0x1210

// Field 2 (4)
#define Si446x_PROP_PKT_FIELD_2_LENGTH_12_8 0x1211
#define Si446x_PROP_PKT_FIELD_2_LENGTH_7_0 0x1212
#define Si446x_PROP_PKT_FIELD_2_CONFIG 0x1213
#define Si446x_PROP_PKT_FIELD_2_CRC_CONFIG 0x1214

// Field 3 (4)
#define Si446x_PROP_PKT_FIELD_3_LENGTH_12_8 0x1215
#define Si446x_PROP_PKT_FIELD_3_LENGTH_7_0 0x1216
#define Si446x_PROP_PKT_FIELD_3_CONFIG 0x1217
#define Si446x_PROP_PKT_FIELD_3_CRC_CONFIG 0x1218

// Field 4 (4)
#define Si446x_PROP_PKT_FIELD_4_LENGTH_12_8 0x1219
#define Si446x_PROP_PKT_FIELD_4_LENGTH_7_0 0x121A
#define Si446x_PROP_PKT_FIELD_4_CONFIG 0x121B
#define Si446x_PROP_PKT_FIELD_4_CRC_CONFIG 0x121C

// Field 5 (4)
#define Si446x_PROP_PKT_FIELD_5_LENGTH_12_8 0x121D
#define Si446x_PROP_PKT_FIELD_5_LENGTH_7_0 0x121E
#define Si446x_PROP_PKT_FIELD_5_CONFIG 0x121F
#define Si446x_PROP_PKT_FIELD_5_CRC_CONFIG 0x1220

// Rx field 1 (4)
#define Si446x_PROP_PKT_RX_FIELD_1_LENGTH_12_8 0x1221
#define Si446x_PROP_PKT_RX_FIELD_1_LENGTH_7_0 0x1222
#define Si446x_PROP_PKT_RX_FIELD_1_CONFIG 0x1223
#define Si446x_PROP_PKT_RX_FIELD_1_CRC_CONFIG 0x1224

// Rx field 2 (4)
#define Si446x_PROP_PKT_RX_FIELD_2_LENGTH_12_8 0x1225
#define Si446x_PROP_PKT_RX_FIELD_2_LENGTH_7_0 0x1226
#define Si446x_PROP_PKT_RX_FIELD_2_CONFIG 0x1227
#define Si446x_PROP_PKT_RX_FIELD_2_CRC_CONFIG 0x1228

// Rx field 3 (4)
#define Si446x_PROP_PKT_RX_FIELD_3_LENGTH_12_8 0x1229
#define Si446x_PROP_PKT_RX_FIELD_3_LENGTH_7_ 0x122A0
#define Si446x_PROP_PKT_RX_FIELD_3_CONFIG 0x122B
#define Si446x_PROP_PKT_RX_FIELD_3_CRC_CONFIG 0x122C

// Rx field 4 (4)
#define Si446x_PROP_PKT_RX_FIELD_4_LENGTH_12_8 0x122D
#define Si446x_PROP_PKT_RX_FIELD_4_LENGTH_7_0 0x122E
#define Si446x_PROP_PKT_RX_FIELD_4_CONFIG 0x122F
#define Si446x_PROP_PKT_RX_FIELD_4_CRC_CONFIG 0x1230

// Rx field 5 (4)
#define Si446x_PROP_PKT_RX_FIELD_5_LENGTH_12_8 0x1231
#define Si446x_PROP_PKT_RX_FIELD_5_LENGTH_7_ 0x1232
#define Si446x_PROP_PKT_RX_FIELD_5_CONFIG 0x1233
#define Si446x_PROP_PKT_RX_FIELD_5_CRC_CONFIG 0x1234

// Modulation and bit mapping (2)
#define Si446x_PROP_MODEM_MOD_TYPE 0x2000
#define Si446x_PROP_MODEM_MAP_CONTROL 0x2001

// Modem data rate (3)
#define Si446x_PROP_MODEM_DATA_RATE_2 0x2003
#define Si446x_PROP_MODEM_DATA_RATE_1 0x2004
#define Si446x_PROP_MODEM_DATA_RATE_0 0x2005

// Modem frequency deviation (3)
#define Si446x_PROP_MODEM_FREQ_DEV_2 0x200A
#define Si446x_PROP_MODEM_FREQ_DEV_1 0x200B
#define Si446x_PROP_MODEM_FREQ_DEV_0 0x200C

// RSSI
#define Si446x_PROP_MODEM_RSSI_THRESH 0x204A
#define Si446x_PROP_MODEM_RSSI_JUMP_THRESH 0x204B
#define Si446x_PROP_MODEM_RSSI_CONTROL 0x204C
#define Si446x_PROP_MODEM_RSSI_CONTROL2 0x204D
#define Si446x_PROP_MODEM_RSSI_COMP 0x204E

// Antenna diversity
#define Si446x_PROP_MODEM_ANT_DIV_CONTROL 0x2049

// Power amplifer (4)
#define Si446x_PROP_PA_MODE 0x2200
#define Si446x_PROP_PA_PWR_LVL 0x2201
#define Si446x_PROP_PA_BIAS_CLKDUTY 0x2202
#define Si446x_PROP_PA_TC 0x2203

// Packet match (12)
#define Si446x_PROP_MATCH_VALUE_1 0x3000
#define Si446x_PROP_MATCH_MASK_1 0x3001
#define Si446x_PROP_MATCH_CTRL_1 0x3002
#define Si446x_PROP_MATCH_VALUE_2 0x3003
#define Si446x_PROP_MATCH_MASK_2 0x3004
#define Si446x_PROP_MATCH_CTRL_2 0x3005
#define Si446x_PROP_MATCH_VALUE_3 0x3006
#define Si446x_PROP_MATCH_MASK_3 0x3007
#define Si446x_PROP_MATCH_CTRL_3 0x3008
#define Si446x_PROP_MATCH_VALUE_4 0x3009
#define Si446x_PROP_MATCH_MASK_4 0x300A
#define Si446x_PROP_MATCH_CTRL_4 0x300B

// Frequency control (6)
#define Si446x_PROP_FREQ_CONTROL_INTE 0x4000
#define Si446x_PROP_FREQ_CONTROL_FRAC_2 0x4001
#define Si446x_PROP_FREQ_CONTROL_FRAC_1 0x4002
#define Si446x_PROP_FREQ_CONTROL_FRAC_0 0x4003
#define Si446x_PROP_FREQ_CONTROL_CHANNEL_STEP_SIZE_1 0x4004
#define Si446x_PROP_FREQ_CONTROL_CHANNEL_STEP_SIZE_0 0x4005

// Rx voltage controlled oscillator adjust (1)
#define Si446x_PROP_FREQ_CONTROL_VCOCNT_RX_ADJ 0x4007

// Rx hop control
#define Si446x_PROP_RX_HOP_CONTROL 0x5000
#define Si446x_PROP_RX_HOP_TABLE_SIZE 0x5001
#define Si446x_PROP_RX_HOP_TABLE_ENTRY_0 0x5002
#define Si446x_PROP_RX_HOP_TABLE_ENTRY_1 0x5003
#define Si446x_PROP_RX_HOP_TABLE_ENTRY_2 0x5004 // so on...
#define Si446x_PROP_RX_HOP_TABLE_ENTRY_63 0x5041

/* Bit configurations */

// RF4463_CMD_GPIO_PIN_CFG
#define RF4463_GPIO_NO_CHANGE                   0
#define RF4463_GPIO_DISABLED                    1
#define RF4463_GPIO_LOW                         2
#define RF4463_GPIO_HIGH                        3
#define RF4463_GPIO_INPUT                       4
#define RF4463_GPIO_32_KHZ_CLOCK                5
#define RF4463_GPIO_BOOT_CLOCK                  6
#define RF4463_GPIO_DIVIDED_MCU_CLOCK           7
#define RF4463_GPIO_CTS                         8
#define RF4463_GPIO_INV_CTS                     9
#define RF4463_GPIO_HIGH_ON_CMD_OVERLAP         10
#define RF4463_GPIO_SPI_DATA_OUT                11
#define RF4463_GPIO_HIGH_AFTER_RESET            12
#define RF4463_GPIO_HIGH_AFTER_CALIBRATION      13
#define RF4463_GPIO_HIGH_AFTER_WUT              14
#define RF4463_GPIO_UNUSED_0                    15
#define RF4463_GPIO_TX_DATA_CLOCK               16
#define RF4463_GPIO_RX_DATA_CLOCK               17
#define RF4463_GPIO_UNUSED_1                    18
#define RF4463_GPIO_TX_DATA                     19
#define RF4463_GPIO_RX_DATA                     20
#define RF4463_GPIO_RX_RAW_DATA                 21
#define RF4463_GPIO_ANTENNA_1_SWITCH            22
#define RF4463_GPIO_ANTENNA_2_SWITCH            23
#define RF4463_GPIO_VALID_PREAMBLE              24
#define RF4463_GPIO_INVALID_PREAMBLE            25
#define RF4463_GPIO_SYNC_DETECTED               26
#define RF4463_GPIO_RSSI_ABOVE_CAT              27
#define RF4463_GPIO_TX_STATE                    32
#define RF4463_GPIO_RX_STATE                    33
#define RF4463_GPIO_RX_FIFO_ALMOST_FULL         34
#define RF4463_GPIO_TX_FIFO_ALMOST_EMPTY        35
#define RF4463_GPIO_BATT_LOW                    36
#define RF4463_GPIO_RSSI_ABOVE_CAT_LOW          37
#define RF4463_GPIO_HOP                         38
#define RF4463_GPIO_HOP_TABLE_WRAPPED           39
#define RF4463_NIRQ_INTERRUPT_SIGNAL			39

// RF4463_CMD_GET_INT_STATUS
#define RF4463_INT_STATUS_CHIP_INT_STATUS                0x04
#define RF4463_INT_STATUS_MODEM_INT_STATUS               0x02
#define RF4463_INT_STATUS_PH_INT_STATUS                  0x01
#define RF4463_INT_STATUS_FILTER_MATCH                   0x80
#define RF4463_INT_STATUS_FILTER_MISS                    0x40
#define RF4463_INT_STATUS_PACKET_SENT                    0x20
#define RF4463_INT_STATUS_PACKET_RX                      0x10
#define RF4463_INT_STATUS_CRC_ERROR                      0x08
#define RF4463_INT_STATUS_TX_FIFO_ALMOST_EMPTY           0x02
#define RF4463_INT_STATUS_RX_FIFO_ALMOST_FULL            0x01
#define RF4463_INT_STATUS_INVALID_SYNC                   0x20
#define RF4463_INT_STATUS_RSSI_JUMP                      0x10
#define RF4463_INT_STATUS_RSSI                           0x08
#define RF4463_INT_STATUS_INVALID_PREAMBLE               0x04
#define RF4463_INT_STATUS_PREAMBLE_DETECT                0x02
#define RF4463_INT_STATUS_SYNC_DETECT                    0x01
#define RF4463_INT_STATUS_CAL                            0x40
#define RF4463_INT_STATUS_FIFO_UNDERFLOW_OVERFLOW_ERROR  0x20
#define RF4463_INT_STATUS_STATE_CHANGE                   0x10
#define RF4463_INT_STATUS_CMD_ERROR                      0x08
#define RF4463_INT_STATUS_CHIP_READY                     0x04
#define RF4463_INT_STATUS_LOW_BATT                       0x02
#define RF4463_INT_STATUS_WUT                            0x01

// RF4463_PROPERTY_INT_CTL_ENABLE
#define RF4463_CHIP_INT_STATUS_EN                        0x04
#define RF4463_MODEM_INT_STATUS_EN                       0x02
#define RF4463_PH_INT_STATUS_EN                          0x01

// RF4463_PROPERTY_FRR_CTL_x_MODE | x: A, B, C, D
#define RF4463_FRR_MODE_DISABLED                         0
#define RF4463_FRR_MODE_GLOBAL_STATUS                    1
#define RF4463_FRR_MODE_GLOBAL_INTERRUPT_PENDING         2
#define RF4463_FRR_MODE_PACKET_HANDLER_STATUS            3
#define RF4463_FRR_MODE_PACKET_HANDLER_INTERRUPT_PENDING 4
#define RF4463_FRR_MODE_MODEM_STATUS                     5
#define RF4463_FRR_MODE_MODEM_INTERRUPT_PENDING          6
#define RF4463_FRR_MODE_CHIP_STATUS                      7
#define RF4463_FRR_MODE_CHIP_INTERRUPT_PENDING           8
#define RF4463_FRR_MODE_CURRENT_STATE                    9
#define RF4463_FRR_MODE_LATCHED_RSSI                     10

// RF4463_PROPERTY_PREAMBLE_CONFIG
#define RF4463_PREAMBLE_STANDARD_PREAM                   0x00
#define RF4463_PREAMBLE_DSA_ONLY                         0x80
#define RF4463_PREAMBLE_FIRST_1                          0x20
#define RF4463_PREAMBLE_FIRST_0                          0x00
#define RF4463_PREAMBLE_LENGTH_NIBBLES                   0x00
#define RF4463_PREAMBLE_LENGTH_BYTES                     0x10
#define RF4463_PREAMBLE_MAN_CONST                        0x08
#define RF4463_PREAMBLE_MAN_ENABLE                       0x02
#define RF4463_PREAMBLE_NON_STANDARD                     0x00
#define RF4463_PREAMBLE_STANDARD_1010                    0x01
#define RF4463_PREAMBLE_STANDARD_0101                    0x02

// RF4463_PROPERTY_SYNC_CONFIG
#define RF4463_SYNC_CONFIG_SKIP_TX                       0x80
#define RF4463_SYNC_CONFIG_RX_ERRORS_MASK                0x70
#define RF4463_SYNC_CONFIG_4FSK                          0x08
#define RF4463_SYNC_CONFIG_MANCH                         0x04
#define RF4463_SYNC_CONFIG_LENGTH_MASK                   0x03

// RF4463_PROPERTY_PKT_CRC_CONFIG
#define RF4463_CRC_SEED_ALL_0S                           0x00
#define RF4463_CRC_SEED_ALL_1S                           0x80
#define RF4463_CRC_MASK                                  0x0f
#define RF4463_CRC_NONE                                  0x00
#define RF4463_CRC_ITU_T                                 0x01
#define RF4463_CRC_IEC_16                                0x02
#define RF4463_CRC_BIACHEVA                              0x03
#define RF4463_CRC_16_IBM                                0x04
#define RF4463_CRC_CCITT                                 0x05
#define RF4463_CRC_KOOPMAN                               0x06
#define RF4463_CRC_IEEE_802_3                            0x07
#define RF4463_CRC_CASTAGNOLI                            0x08

// define RF4463_PROPERTY_PKT_CONFIG1
#define RF4463_PH_FIELD_SPLIT                            0x80
#define RF4463_PH_RX_DISABLE                             0x40
#define RF4463_4FSK_EN                                   0x20
#define RF4463_RX_MULTI_PKT                              0x10
#define RF4463_MANCH_POL                                 0x08
#define RF4463_CRC_INVERT                                0x04
#define RF4463_CRC_ENDIAN                                0x02
#define RF4463_BIT_ORDER                                 0x01

// RF4463_PROPERTY_PKT_LEN
#define RF4463_INFINITE_LEN  							0x40
#define RF4463_ENDIAN_LITTLE  							0x00
#define RF4463_ENDIAN_BIG  								0x20
#define RF4463_SIZE_ENUM_0 								0x00
#define RF4463_SIZE_ENUM_1 								0x01
#define RF4463_IN_FIFO 									0x08
#define RF4463_DST_FIELD_ENUM_0 						0x00
#define RF4463_DST_FIELD_ENUM_1 						0x01
#define RF4463_DST_FIELD_ENUM_2 						0x02
#define RF4463_DST_FIELD_ENUM_3 						0x03
#define RF4463_DST_FIELD_ENUM_4 						0x04
#define RF4463_DST_FIELD_ENUM_5 						0x05
#define RF4463_DST_FIELD_ENUM_6 						0x06
#define RF4463_DST_FIELD_ENUM_7 						0x07

// RF4463_PROPERTY_PKT_LEN_FIELD_SOURCE
#define RF4463_SRC_FIELD_ENUM_0 						0x00
#define RF4463_SRC_FIELD_ENUM_1 						0x01
#define RF4463_SRC_FIELD_ENUM_2 						0x02
#define RF4463_SRC_FIELD_ENUM_3 						0x03
#define RF4463_SRC_FIELD_ENUM_4 						0x04
#define RF4463_SRC_FIELD_ENUM_5 						0x05

// RF4463_PROPERTY_PKT_FIELD_x_CONFIG | x: 1,2,3,4,5
#define RF4463_FIELD_CONFIG_4FSK                         0x10
#define RF4463_FIELD_CONFIG_PN_START                     0x04
#define RF4463_FIELD_CONFIG_WHITEN                       0x02
#define RF4463_FIELD_CONFIG_MANCH                        0x01

//#define RF4463_PROPERTY_PKT_RX_FIELD_x_CRC_CONFIG | x: 1,2,3,4,5
#define RF4463_FIELD_CONFIG_CRC_START                     0x80
#define RF4463_FIELD_CONFIG_ALT_CRC_START                 0x40
#define RF4463_FIELD_CONFIG_SEND_CRC                      0x20
#define RF4463_FIELD_CONFIG_SEND_ALT_CRC                  0x10
#define RF4463_FIELD_CONFIG_CHECK_CRC                     0x08
#define RF4463_FIELD_CONFIG_CHECK_ALT_CRC                 0x04
#define RF4463_FIELD_CONFIG_CRC_ENABLE                    0x02
#define RF4463_FIELD_CONFIG_ALT_CRC_ENABLE                0x01

// RF4463_PROPERTY_MODEM_MOD_TYPE
#define RF4463_TX_DIRECT_MODE_TYPE_SYNCHRONOUS           0x00
#define RF4463_TX_DIRECT_MODE_TYPE_ASYNCHRONOUS          0x80
#define RF4463_TX_DIRECT_MODE_GPIO0                      0x00
#define RF4463_TX_DIRECT_MODE_GPIO1                      0x20
#define RF4463_TX_DIRECT_MODE_GPIO2                      0x40
#define RF4463_TX_DIRECT_MODE_GPIO3                      0x60
#define RF4463_MOD_SOURCE_PACKET_HANDLER                 0x00
#define RF4463_MOD_SOURCE_DIRECT_MODE                    0x08
#define RF4463_MOD_SOURCE_RANDOM_GENERATOR               0x10
#define RF4463_MOD_TYPE_CW                               0x00
#define RF4463_MOD_TYPE_OOK                              0x01
#define RF4463_MOD_TYPE_2FSK                             0x02
#define RF4463_MOD_TYPE_2GFSK                            0x03
#define RF4463_MOD_TYPE_4FSK                             0x04
#define RF4463_MOD_TYPE_4GFSK                            0x05

// RF4463_PROPERTY_PA_MODE
#define RF4463_PA_MODE_1_GROUP                           0x04
#define RF4463_PA_MODE_2_GROUPS                          0x08
#define RF4463_PA_MODE_CLASS_E                           0x00
#define RF4463_PA_MODE_SWITCH_CURRENT                    0x01

#endif // si446x_defs.h
