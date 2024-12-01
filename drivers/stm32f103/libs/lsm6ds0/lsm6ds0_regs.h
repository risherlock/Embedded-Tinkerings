#ifndef _LSM6DS0_SETTINGS_H_
#define _LSM6DS0_SETTINGS_H_

#define LSM6DS0_I2C_SLAVE_ADD 0x6B

// WHO_AM_I & response
#define LSM6DS0_WHO_AM_I_VAL 0x6C
#define LSM6DS0_WHO_AM_I 0x0F

// Output  registers
#define LSM6DS0_OUT_X_L_XL 0x28
#define LSM6DS0_OUT_X_L_G 0x22
#define LSM6DS0_OUT_TEMP_L 0x20

// Control registers
#define LSM6DS0_CTRL1_XL 0x10
#define LSM6DS0_CTRL2_G 0x11

// Linear Acceleration: milli g's per LSB
#define LSM6DS0_ACCEL_mg_LSB_2G 0.061F  // +-2g
#define LSM6DS0_ACCEL_mg_LSB_4G 0.122F  // +-4g
#define LSM6DS0_ACCEL_mg_LSB_6G 0.183F  // +-6g
#define LSM6DS0_ACCEL_mg_LSB_8G 0.244F  // +-8g
#define LSM6DS0_ACCEL_mg_LSB_16G 0.732F // +-16g
#define LSM6DS0_GRAVITY_EARTH 9.80665F  // m/s^2

// Gyroscope omega: dps per LSB
#define LSM6DS0_GYRO_DPS_DIGIT_245DPS 0.00875F  // +-245dps
#define LSM6DS0_GYRO_DPS_DIGIT_500DPS 0.01750F  // +-500dps
#define LSM6DS0_GYRO_DPS_DIGIT_2000DPS 0.07000F // +-2000dps

// Magnetic field strength: milli Gauss per LSB
#define LSM6DS0_MAG_mG_2GAUSS 0.08F  // +-2 gauss
#define LSM6DS0_MAG_mG_4GAUSS 0.16F  // +-4 gauss
#define LSM6DS0_MAG_mG_8GAUSS 0.32F  // +-8 gauss
#define LSM6DS0_MAG_mG_12GAUSS 0.48F // +-12 gauss

#endif // lsm9ds1_settings.h
