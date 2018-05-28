#include "msp.h"
#include "delay.h"
#include "led.h"
#include "battery.h"
#include "imu.h"
#include "rc.h"
#include "esc.h"
#include <math.h>

#include "lcd.h"
unsigned char count;

int angle[3];

volatile sensor s;
volatile channel ch[6];

// read accelerometer and gyroscope data
void angle_read()
{
    // read accelerometer (X,Y,Z) values
    s.accel[X] = (read_imu(ACCEL_XOUT_H,&s) << 8) | read_imu(ACCEL_XOUT_L,&s);
    s.accel[Y] = (read_imu(ACCEL_YOUT_H,&s) << 8) | read_imu(ACCEL_YOUT_L,&s);
    s.accel[Z] = (read_imu(ACCEL_ZOUT_H,&s) << 8) | read_imu(ACCEL_ZOUT_L,&s);

    // read gyroscope (X,Y,Z) values
    s.gyro[X] = ((read_imu(GYRO_XOUT_H,&s) << 8) | read_imu(GYRO_XOUT_L,&s)) - s.offset[X];
    s.gyro[Y] = ((read_imu(GYRO_YOUT_H,&s) << 8) | read_imu(GYRO_YOUT_L,&s)) - s.offset[Y];
    s.gyro[Z] = ((read_imu(GYRO_ZOUT_H,&s) << 8) | read_imu(GYRO_ZOUT_L,&s)) - s.offset[Z];
}

// calculate angles using sensor data
void angle_calc()
{
    int accel_angle[3];
    int gyro_angle[3];

    // calculate angle based on accelerometer data
    accel_angle[ROLL] = ((atan2(s.accel[Y],sqrt((s.accel[X] * s.accel[X]) + (s.accel[Z] * s.accel[Z]))) * 180 / M_PI) - ROLL_OFFSET) * UINT16_MAX;
    accel_angle[PITCH] = ((atan2(s.accel[X],sqrt((s.accel[Y] * s.accel[Y]) + (s.accel[Z] * s.accel[Z]))) * 180 / M_PI) - PITCH_OFFSET) * UINT16_MAX;

    // calculate angle based on gyroscope data
    gyro_angle[ROLL] = angle[ROLL] + (s.gyro[X] * UINT16_MAX / GYRO_SCALE / IMU_RATE);
    gyro_angle[PITCH] = angle[PITCH] + (s.gyro[Y] * UINT16_MAX / GYRO_SCALE / IMU_RATE);
    gyro_angle[YAW] = s.gyro[Z] * UINT16_MAX / GYRO_SCALE;

    // couple roll and pitch using yaw
    gyro_angle[ROLL] -= gyro_angle[PITCH] * sin(gyro_angle[YAW] / UINT16_MAX / IMU_RATE * M_PI / 180);
    gyro_angle[PITCH] += gyro_angle[ROLL] * sin(gyro_angle[YAW] / UINT16_MAX / IMU_RATE * M_PI / 180);

    // combine gyroscope and accelerometer using a complementary filter
    angle[ROLL] = ALPHA * gyro_angle[ROLL] + (1 - ALPHA) * accel_angle[ROLL];
    angle[PITCH] = ALPHA * gyro_angle[PITCH] + (1 - ALPHA) * accel_angle[PITCH];
    angle[YAW] = BETA * angle[YAW] + (1 - BETA) * gyro_angle[YAW];
}

// system initialization
void init()
{
    int i;

    // disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize components
    init_dco();
    init_led();
    //init_battery();
    init_imu(&s);
    init_rc(ch);
    init_esc();
    init_lcd();

    // enable all interrupts
    __enable_irq();

    // setup IMU configuration registers
    write_imu(PWR_MGMT_1,PWR_MGMT_1_CLKSEL_1,&s);
    write_imu(CONFIG,CONFIG_DLPF_CFG_3,&s);
    write_imu(ACCEL_CONFIG,ACCEL_CONFIG_AFS_SEL_2,&s);
    write_imu(GYRO_CONFIG,GYRO_CONFIG_FS_SEL_1,&s);

    // calibrate gyroscope
    for(i = 0; i < IMU_CAL; i++)
    {
        s.offset[X] += (read_imu(GYRO_XOUT_H,&s) << 8) | read_imu(GYRO_XOUT_L,&s);
        s.offset[Y] += (read_imu(GYRO_YOUT_H,&s) << 8) | read_imu(GYRO_YOUT_L,&s);
        s.offset[Z] += (read_imu(GYRO_ZOUT_H,&s) << 8) | read_imu(GYRO_ZOUT_L,&s);

        if(i & BIT7)
        {
            red_led();
        }
    }

    s.offset[X] /= IMU_CAL;
    s.offset[Y] /= IMU_CAL;
    s.offset[Z] /= IMU_CAL;
    reset_led();
    clear_lcd();

    // start TimerA
    TIMER_A1->CTL |= TIMER_A_CTL_MC__UP;
}

// main program
void main()
{
    init();
    green_led();

    while(1)
    {
        while(!s.sample_flag);
        s.sample_flag = 0;
        count++;

        angle_read();
        angle_calc();

        if(count == 10)
        {
            clear_lcd();
            count = 0;

            write_string_lcd("ROLL: ");
            write_int_lcd(angle[ROLL] / UINT16_MAX);

            write_string_lcd("\nPITCH: ");
            write_int_lcd(angle[PITCH] / UINT16_MAX);
            write_string_lcd("      ");

            /*
            write_string_lcd("\nYAW:  ");
            write_int_lcd(angle[YAW] / UINT16_MAX);
            write_string_lcd("       ");
            */
        }
    }
}

// ADC14 interrupt service routine
void ADC14_IRQHandler()
{
    read_battery();
}

// Timer32 interrupt service routine
void T32_INT2_IRQHandler()
{
    alert_battery();
}

// I2C interrupt service routine
void EUSCIB0_IRQHandler()
{
    i2c_imu(&s);
}

// TimerA interrupt service routine
void TA1_0_IRQHandler()
{
    sample_imu(&s);
}

// P3 interrupt service routine
void PORT3_IRQHandler()
{
    process_rc(ch);
}
