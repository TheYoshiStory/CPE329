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
#define IMU_CAL 4096
#define IMU_RATE 100

#define PWR_MGMT_1 0x6B
#define CONFIG 0x1A
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG 0x1B
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define PWR_MGMT_1_CLKSEL_1 0x01
#define CONFIG_DLPF_CFG_3 0x03
#define ACCEL_CONFIG_AFS_SEL_2 0x10
#define GYRO_CONFIG_FS_SEL_1 0x08

#define X 0
#define Y 1
#define Z 2

#define ROLL 0
#define PITCH 1
#define YAW 2

#define ROLL_OFFSET 0
#define PITCH_OFFSET 0

#define ACCEL_SCALE 417.959
#define GYRO_SCALE 65.500

#define ALPHA  0.900
#define BETA 0.100

void init_imu();

#endif
