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

volatile char i2c_flag;
volatile char sample_flag;
volatile channel ch[6];

int offset[3];
int angle[3];

void i2c_write(unsigned char reg, unsigned char data)
{
    // set transmit mode and send START condition
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
    while(!i2c_flag);
    i2c_flag = 0;

    // send register address
    EUSCI_B0->TXBUF = reg;
    while(!i2c_flag);
    i2c_flag = 0;

    // send register data
    EUSCI_B0->TXBUF = data;
    while(!i2c_flag);
    i2c_flag = 0;

    // send STOP condition
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;
}

unsigned char i2c_read(unsigned char reg)
{
    // set transmit mode and send START condition
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
    while(!i2c_flag);
    i2c_flag = 0;

    // send register address
    EUSCI_B0->TXBUF = reg;
    while(!i2c_flag);
    i2c_flag = 0;

    // set receive mode and send RESTART condition
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
    while((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    // wait to receive data
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    while(!i2c_flag);
    i2c_flag = 0;

    return(EUSCI_B0->RXBUF & 0xFF);
}

// calculate angles using accelerometer and gyroscope data
void angle_calc()
{
    short accel_data[3];
    short gyro_data[3];
    int accel_angle[3];
    int gyro_angle[3];

    // read accelerometer (X,Y,Z) values
    accel_data[X] = (i2c_read(ACCEL_XOUT_H) << 8) | i2c_read(ACCEL_XOUT_L);
    accel_data[Y] = (i2c_read(ACCEL_YOUT_H) << 8) | i2c_read(ACCEL_YOUT_L);
    accel_data[Z] = (i2c_read(ACCEL_ZOUT_H) << 8) | i2c_read(ACCEL_ZOUT_L);

    // read gyroscope (X,Y,Z) values
    gyro_data[X] = ((i2c_read(GYRO_XOUT_H) << 8) | i2c_read(GYRO_XOUT_L)) - offset[X];
    gyro_data[Y] = ((i2c_read(GYRO_YOUT_H) << 8) | i2c_read(GYRO_YOUT_L)) - offset[Y];
    gyro_data[Z] = ((i2c_read(GYRO_ZOUT_H) << 8) | i2c_read(GYRO_ZOUT_L)) - offset[Z];

    // calculate angle based on accelerometer data
    accel_angle[ROLL] = ((atan2(accel_data[Y],sqrt((accel_data[X] * accel_data[X]) + (accel_data[Z] * accel_data[Z]))) * 180 / M_PI) - ROLL_OFFSET) * UINT16_MAX;
    accel_angle[PITCH] = ((atan2(accel_data[X],sqrt((accel_data[Y] * accel_data[Y]) + (accel_data[Z] * accel_data[Z]))) * 180 / M_PI) - PITCH_OFFSET) * UINT16_MAX;

    // calculate angle based on gyroscope data
    gyro_angle[ROLL] = angle[ROLL] + (gyro_data[X] * UINT16_MAX / GYRO_SCALE / IMU_RATE);
    gyro_angle[PITCH] = angle[PITCH] + (gyro_data[Y] * UINT16_MAX / GYRO_SCALE / IMU_RATE);
    gyro_angle[YAW] = gyro_data[Z] * UINT16_MAX / GYRO_SCALE;

    // couple roll and pitch using yaw
    gyro_angle[ROLL] -= gyro_angle[PITCH] * sin(gyro_angle[YAW] / UINT16_MAX / IMU_RATE * M_PI / 180);
    gyro_angle[PITCH] += gyro_angle[ROLL] * sin(gyro_angle[YAW] / UINT16_MAX / IMU_RATE * M_PI / 180);

    // check for receiver enable
    if(1)
    {
        // combine gyroscope and accelerometer angles with a complementary filter
        angle[ROLL] = ALPHA * gyro_angle[ROLL] + (1 - ALPHA) * accel_angle[ROLL];
        angle[PITCH] = ALPHA * gyro_angle[PITCH] + (1 - ALPHA) * accel_angle[PITCH];
        angle[YAW] = BETA * angle[YAW] + (1 - BETA) * gyro_angle[YAW];
    }
    else
    {
        // initialize IMU with accelerometer angles
        angle[ROLL] = accel_angle[ROLL];
        angle[PITCH] = accel_angle[PITCH];
        angle[YAW] = 0;
    }
}

// system initialization
void init()
{
    int i;

    // disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize interrupt flags
    i2c_flag = 0;
    sample_flag = 0;

    // initialize components
    init_dco();
    init_led();
    init_imu();
    //init_battery();
    init_rc(ch);
    init_esc();
    init_lcd();

    // enable all interrupts
    __enable_irq();

    // setup IMU configuration registers
    i2c_write(PWR_MGMT_1,PWR_MGMT_1_CLKSEL_1);
    i2c_write(CONFIG,CONFIG_DLPF_CFG_3);
    i2c_write(ACCEL_CONFIG,ACCEL_CONFIG_AFS_SEL_2);
    i2c_write(GYRO_CONFIG,GYRO_CONFIG_FS_SEL_1);

    // calibrate gyroscope
    for(i = 0; i < IMU_CAL; i++)
    {
        offset[X] += (i2c_read(GYRO_XOUT_H) << 8) | i2c_read(GYRO_XOUT_L);
        offset[Y] += (i2c_read(GYRO_YOUT_H) << 8) | i2c_read(GYRO_YOUT_L);
        offset[Z] += (i2c_read(GYRO_ZOUT_H) << 8) | i2c_read(GYRO_ZOUT_L);

        if(i & BIT7)
        {
            red_led();
        }
    }

    offset[X] /= IMU_CAL;
    offset[Y] /= IMU_CAL;
    offset[Z] /= IMU_CAL;
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

        angle_calc();

        if(count == 10)
        {
            clear_lcd();
            count = 0;

            write_string_lcd("ROLL:  ");
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

// I2C interrupt service routine
void EUSCIB0_IRQHandler()
{
    // check if byte was successfully transmitted
    if(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0)
    {
        i2c_flag = 1;
        EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;
    }

    // check if byte was successfully received
    if(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0)
    {
        i2c_flag = 1;
        EUSCI_B0->IFG &= ~EUSCI_B_IFG_RXIFG0;
    }
}

// TimerA interrupt service routine
void TA1_0_IRQHandler()
{
    // set sample flag and clear interrupt flag
    sample_flag = 1;
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
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

// P3 interrupt service routine
void PORT3_IRQHandler()
{
    process_rc(ch);
}
