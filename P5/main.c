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

#define SAMPLE_RATE 100

volatile sensor s;
volatile channel ch[6];
volatile char sample_flag;

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
    gyro_angle[ROLL] = s.angle[ROLL] + (s.gyro[X] * UINT16_MAX / GYRO_SCALE / SAMPLE_RATE);
    gyro_angle[PITCH] = s.angle[PITCH] + (s.gyro[Y] * UINT16_MAX / GYRO_SCALE / SAMPLE_RATE);
    gyro_angle[YAW] = s.gyro[Z] * UINT16_MAX / GYRO_SCALE / SAMPLE_RATE;

    // couple roll and pitch using yaw
    gyro_angle[ROLL] -= gyro_angle[PITCH] * sin(gyro_angle[YAW] / UINT16_MAX * M_PI / 180);
    gyro_angle[PITCH] += gyro_angle[ROLL] * sin(gyro_angle[YAW] / UINT16_MAX * M_PI / 180);

    // combine gyroscope and accelerometer using a complementary filter
    s.angle[ROLL] = ALPHA * gyro_angle[ROLL] + (1 - ALPHA) * accel_angle[ROLL];
    s.angle[PITCH] = ALPHA * gyro_angle[PITCH] + (1 - ALPHA) * accel_angle[PITCH];
    s.angle[YAW] = BETA * s.angle[YAW] + (1 - BETA) * s.gyro[Z] / GYRO_SCALE;
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
    //init_rc(ch);
    //init_esc();
    init_lcd();

    // configure TimerA for 1ms interval timing
    TIMER_A1->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID__4 | TIMER_A_CTL_MC__STOP |TIMER_A_CTL_CLR | TIMER_A_CTL_IE;
    TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A1->CCR[0] = CLK_FREQ / 4 / SAMPLE_RATE;
    NVIC->ISER[0] = 1 << ((TA1_0_IRQn) & 31);

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
        while(!sample_flag);
        sample_flag = 0;
        count++;

        angle_read();
        angle_calc();

        if(count == 10)
        {
            clear_lcd();
            write_string_lcd("ROLL:  ");
            write_int_lcd(s.angle[ROLL] / UINT16_MAX);
            write_string_lcd("\nPITCH: ");
            write_int_lcd(s.angle[PITCH] / UINT16_MAX);
            write_string_lcd("      ");
            count = 0;
        }
    }
}


// TimerA interrupt service routine
void TA1_0_IRQHandler()
{
    // clear interrupt flag and set sample flag
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    sample_flag = 1;
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

// P3 interrupt service routine
void PORT3_IRQHandler()
{
    process_rc(ch);
}
