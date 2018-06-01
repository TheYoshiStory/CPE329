#include "msp.h"
#include "delay.h"
#include "led.h"
#include "imu.h"
#include "battery.h"
#include "rc.h"
#include "esc.h"
#include <math.h>

#include "uart.h"

#define P 0
#define I 1
#define D 2

#define P_ROLL_GAIN 1
#define I_ROLL_GAIN 0
#define D_ROLL_GAIN 0

#define P_PITCH_GAIN 1
#define I_PITCH_GAIN 0
#define D_PITCH_GAIN 0

#define P_YAW_GAIN 1
#define I_YAW_GAIN 0
#define D_YAW_GAIN 0

volatile char i2c_flag;
volatile char sample_flag;
volatile channel ch[6];

int output[3][3];
int offset[3];
int angle[3];

int prev[3];

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
    accel_angle[PITCH] = -1 * ((atan2(accel_data[X],sqrt((accel_data[Y] * accel_data[Y]) + (accel_data[Z] * accel_data[Z]))) * 180 / M_PI) - PITCH_OFFSET) * UINT16_MAX;

    // calculate angle based on gyroscope data
    gyro_angle[ROLL] = angle[ROLL] + (gyro_data[X] * UINT16_MAX / GYRO_SCALE / IMU_RATE);
    gyro_angle[PITCH] = angle[PITCH] + (-1 * gyro_data[Y] * UINT16_MAX / GYRO_SCALE / IMU_RATE);
    gyro_angle[YAW] = -1 * gyro_data[Z] * UINT16_MAX / GYRO_SCALE;

    // couple roll and pitch using yaw
    gyro_angle[ROLL] -= gyro_angle[PITCH] * sin(gyro_angle[YAW] / UINT16_MAX / IMU_RATE * M_PI / 180);
    gyro_angle[PITCH] += gyro_angle[ROLL] * sin(gyro_angle[YAW] / UINT16_MAX / IMU_RATE * M_PI / 180);

    // check for receiver enable
    if(ch[4].pulse > RC_THRESH)
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

// print angle values to UART console for debugging
void angle_print()
{
    short data;

    // print roll angle
    tx_uart('R');
    tx_uart('O');
    tx_uart('L');
    tx_uart('L');
    tx_uart(':');
    tx_uart(' ');

    data = (ch[0].pulse - angle[ROLL]) / UINT16_MAX;

    if(data < 0)
    {
        tx_uart('-');
        data *= -1;
    }
    else
    {
        tx_uart('+');
    }

    tx_uart(data / 10 + 48);
    data -= data / 10 * 10;
    tx_uart(data + 48);
    tx_uart('d');
    tx_uart('e');
    tx_uart('g');
    tx_uart(0x09);

    // print pitch angle
    tx_uart('P');
    tx_uart('I');
    tx_uart('T');
    tx_uart('C');
    tx_uart('H');
    tx_uart(':');
    tx_uart(' ');

    data = (ch[1].pulse - angle[PITCH]) / UINT16_MAX;

    if(data < 0)
    {
        tx_uart('-');
        data *= -1;
    }
    else
    {
        tx_uart('+');
    }

    tx_uart(data / 10 + 48);
    data -= data / 10 * 10;
    tx_uart(data + 48);
    tx_uart('d');
    tx_uart('e');
    tx_uart('g');
    tx_uart(0x09);

    tx_uart('Y');
    tx_uart('A');
    tx_uart('W');
    tx_uart(':');
    tx_uart(' ');

    // print yaw angular velocity
    data = (ch[3].pulse - angle[YAW]) / UINT16_MAX;

    if(data < 0)
    {
        tx_uart('-');
        data *= -1;
    }
    else
    {
        tx_uart('+');
    }

    tx_uart(data / 100 + 48);
    data -= data / 100 * 100;
    tx_uart(data / 10 + 48);
    data -= data / 10 * 10;
    tx_uart(data + 48);
    tx_uart('d');
    tx_uart('e');
    tx_uart('g');
    tx_uart('/');
    tx_uart('s');
    tx_uart(0x0D);
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
    init_battery();
    init_rc(ch);
    init_esc();

    // use UART for debugging
    //init_uart();

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
    clc_uart();

    // start TimerA
    TIMER_A1->CTL |= TIMER_A_CTL_MC__UP;
}

int saturate(int val, int min, int max)
{
    if (val < min)
    {
        return min;
    }
    else if (val > max)
    {
        return max;
    }
    else
    {
        return val;
    }
}

void receiver_calc()
{
    ch[0].set_point = ((ch[0].pulse * RC_MAX_ANGLE / 12000) - 135) * UINT16_MAX;
    ch[1].set_point = ((ch[1].pulse * RC_MAX_ANGLE / 12000) - 135) * UINT16_MAX;
    ch[2].set_point = (ch[2].pulse) / 8;
    ch[3].set_point = ((ch[3].pulse * RC_MAX_ANGLE / 12000) - 135) * UINT16_MAX;
    ch[4].set_point = ch[4].pulse;
    ch[5].set_point = ch[5].pulse;
}

void pid_calc()
{
    int error[3];

    if (ch[4].set_point > RC_THRESH)
    {
        error[ROLL] = ch[0].pulse - angle[ROLL];
        error[PITCH] = ch[1].pulse - angle[PITCH];
        error[YAW] = ch[3].pulse - angle[YAW];

        output[P][ROLL] = P_ROLL_GAIN * error[ROLL];
        output[P][PITCH] = P_PITCH_GAIN * error[PITCH];
        output[P][YAW] = P_YAW_GAIN * error[YAW];

        output[I][ROLL] += I_ROLL_GAIN * error[ROLL] / IMU_RATE;
        output[I][ROLL] = saturate(output[I][ROLL], -1 * RC_MAX_ANGLE * UINT16_MAX * I_ROLL_GAIN, RC_MAX_ANGLE * UINT16_MAX * I_ROLL_GAIN);
        output[I][PITCH] += I_PITCH_GAIN * error[PITCH] / IMU_RATE;
        output[I][PITCH] = saturate(output[I][PITCH], -1 * RC_MAX_ANGLE * UINT16_MAX * I_PITCH_GAIN, RC_MAX_ANGLE * UINT16_MAX * I_PITCH_GAIN);
        output[I][YAW] += I_YAW_GAIN * error[YAW] / IMU_RATE;
        output[I][YAW] = saturate(output[I][YAW], -1 * RC_MAX_ANGLE * UINT16_MAX * I_YAW_GAIN, RC_MAX_ANGLE * UINT16_MAX * I_YAW_GAIN);

        output[D][ROLL] = D_ROLL_GAIN * (error[ROLL] - prev[ROLL]) * IMU_RATE;
        output[D][PITCH] = D_PITCH_GAIN * (error[PITCH] - prev[PITCH]) * IMU_RATE;
        output[D][YAW] = D_YAW_GAIN * (error[YAW] - prev[YAW]) * IMU_RATE;

        prev[ROLL] = error[ROLL];
        prev[PITCH] = error[PITCH];
        prev[YAW] = error[YAW];
    }
    else
    {
        output[I][ROLL] = 0;
        output[I][PITCH] = 0;
        output[I][YAW] = 0;

        prev[ROLL] = 0;
        prev[PITCH] = 0;
        prev[YAW] = 0;
    }
}

// main program
void main()
{
    init();
    green_led();
    int error[3];
    int esc[4];


    while(1)
    {
        while(!sample_flag);
        sample_flag = 0;

        receiver_calc();
        angle_calc();
        pid_calc();
        //angle_print();

        error[ROLL] = (2800 * (output[P][ROLL] + output[I][ROLL] + output[D][ROLL]) / 90 / UINT16_MAX);
        error[PITCH] = (2800 * (output[P][PITCH] + output[I][PITCH] + output[D][PITCH]) / 90 / UINT16_MAX);
        error[YAW] = (2800 * (output[P][YAW] + output[I][YAW] + output[D][YAW]) / 90 / UINT16_MAX);

        //Normalize outputs
        esc[LF] = saturate(ch[2].pulse + (error[ROLL]) + (error[PITCH]) + (error[YAW]), 3200, 6000);
        esc[RF] = saturate(ch[2].pulse - (error[ROLL]) + (error[PITCH]) - (error[YAW]), 3200, 6000);
        esc[LB] = saturate(ch[2].pulse - (error[ROLL]) - (error[PITCH]) + (error[YAW]), 3200, 6000);
        esc[RB] = saturate(ch[2].pulse + (error[ROLL]) - (error[PITCH]) - (error[YAW]), 3200, 6000);

        if (ch[4].pulse > RC_THRESH)
        {
            TIMER_A0->CCR[1] = esc[LF];
            TIMER_A0->CCR[2] = esc[RF];
            TIMER_A0->CCR[3] = esc[LB];
            TIMER_A0->CCR[4] = esc[RB];
        }
        else
        {
            TIMER_A0->CCR[1] = 3000;
            TIMER_A0->CCR[2] = 3000;
            TIMER_A0->CCR[3] = 3000;
            TIMER_A0->CCR[4] = 3000;
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
    if (ch[5].pulse > RC_THRESH)
    {
        alert_battery();
    }
    else
    {
        BATTERY_CTRL->OUT &= ~BIT1;
        TIMER32_2->INTCLR++;
    }
}

// P3 interrupt service routine
void PORT3_IRQHandler()
{
    process_rc(ch);
}
