#ifndef _LSM6DS0_SETTINGS_H_
#define _LSM6DS0_SETTINGS_H_

// WHO_AM_I & responses
#define LSM6DS0_WHO_AM_I 0x0F
#define LSM6DS0_WHO_AM_I_VAL 0x6C
#define LSM6DS0_G_RESPONSE 0x68
#define LSM6DS0_XM_RESPONSE 0x3D

// I2C address
#define LSM6DS0_ADDR_AG 0x6B
#define LSM6DS0_ADDR_M 0x1E

// Output & calibration registers
#define LSM6DS0_OUT_X_L_XL 0x28
#define LSM6DS0_OUT_X_L_G 0x18
#define LSM6DS0_OUT_X_L_M 0x28
#define LSM6DS0_OFFSET_X_REG_L_M 0x05

// Gyro control registers
#define LSM6DS0_CTRL_REG1_G 0x10
#define LSM6DS0_CTRL_REG2_G 0x11
#define LSM6DS0_CTRL_REG3_G 0x12
#define LSM6DS0_CTRL_REG4 0x1E

// Accelerometer control registers
#define LSM6DS0_CTRL_REG5_XL 0x1F
#define LSM6DS0_CTRL_REG6_XL 0x20
#define LSM6DS0_CTRL_REG7_XL 0x21

// Magnetometer control registers
#define LSM6DS0_CTRL_REG1_M 0x20
#define LSM6DS0_CTRL_REG2_M 0x21
#define LSM6DS0_CTRL_REG3_M 0x22
#define LSM6DS0_CTRL_REG4_M 0x23
#define LSM6DS0_CTRL_REG5_M 0x24

// General control registers
#define LSM6DS0_CTRL_REG8 0x22
#define LSM6DS0_CTRL_REG9 0x23
#define LSM6DS0_CTRL_REG10 0x24

// Status registers
#define LSM6DS0_STATUS_REG_G 0x17
#define LSM6DS0_STATUS_REG_XL 0x26
#define LSM6DS0_STATUS_REG_M 0x27

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