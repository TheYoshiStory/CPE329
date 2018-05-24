#ifndef IMU_H
#define IMU_H

/*
 *  -------------------------------------
 *  | SCL | SDA | - | - | - | - | - | - |
 *  -------------------------------------
 */
#define IMU_CTRL P1

#define IMU_ADDR 0x68
#define IMU_FREQ 400000

void init_imu();
void write_imu(unsigned char reg, unsigned char data);
unsigned char read_imu(unsigned char reg);

#endif
