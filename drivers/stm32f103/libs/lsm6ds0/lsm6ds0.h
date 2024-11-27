// 2024-11-25

#ifndef LSM6DS0_H_
#define LSM6DS0_H_

#include <stdbool.h>

bool lsm6ds0_init(void);
bool lsm6ds0_get_accel(float a[3]);
bool lsm6ds0_get_gyro(float g[3]);
float get_temperature(void);

#endif // lsm6ds0.h

