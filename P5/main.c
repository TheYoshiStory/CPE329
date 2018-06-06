#include "msp.h"
#include "delay.h"
#include "led.h"
#include "esc.h"
#include "rc.h"
#include "imu.h"
#include "battery.h"
#include <math.h>

// ----------------------------------------------------------------------------
// -- CONSTANTS ---------------------------------------------------------------
// ----------------------------------------------------------------------------
#define INPUT_SCALE 8.0
#define OUTPUT_SCALE 1500.0
#define MAX_ANGLE 90.0

#define P_ROLL_GAIN 1.0
#define I_ROLL_GAIN 0.2
#define D_ROLL_GAIN 0.2

#define P_PITCH_GAIN 1.0
#define I_PITCH_GAIN 0.2
#define D_PITCH_GAIN 0.2

#define P_YAW_GAIN 1.5
#define I_YAW_GAIN 0.0
#define D_YAW_GAIN 0.0
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// -- GLOBAL VARIABLES --------------------------------------------------------
// ----------------------------------------------------------------------------
volatile char i2c_flag;
volatile char sample_flag;
volatile channel ch[6];

float angle[3];
float offset[3];

float pid[3];
float sum[3];
float prev[3];

float esc[4];
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// -- UTILITY FUNCTIONS -------------------------------------------------------
// ----------------------------------------------------------------------------
float saturate(float val, float min, float max)
{
    // saturate input between two extrema
    if(val < min)
    {
        return(min);
    }
    else if(val > max)
    {
        return(max);
    }
    else
    {
        return(val);
    }
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

    // return data in buffer
    return(EUSCI_B0->RXBUF & 0xFF);
}

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
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// -- CONTROLLER FUNCTIONS ----------------------------------------------------
// ----------------------------------------------------------------------------
void input_calc()
{
    // linearize roll with a range of MAX_ANGLE
    if(abs(ch[0].pulse - RC_MID) > RC_DZ)
    {
        ch[0].setpoint = MAX_ANGLE / 2 * (ch[0].pulse - RC_MID) / (RC_MAX - RC_MID);
    }
    else
    {
        ch[0].setpoint = 0;
    }

    // linearize pitch with a range of MAX_ANGLE
    if(abs(ch[1].pulse - RC_MID) > RC_DZ)
    {
        ch[1].setpoint = MAX_ANGLE / 2 * (ch[1].pulse - RC_MID) / (RC_MAX - RC_MID);
    }
    else
    {
        ch[1].setpoint = 0;
    }

    // linearize yaw with a range of MAX_ANGLE
    if(abs(ch[3].pulse - RC_MID) > RC_DZ)
    {
        ch[3].setpoint = MAX_ANGLE / 2 * (ch[3].pulse - RC_MID) / (RC_MAX - RC_MID);
    }
    else
    {
        ch[3].setpoint = 0;
    }

    // put throttle input on same scale as ESC inputs
    ch[2].setpoint = ch[2].pulse / INPUT_SCALE;

    // use raw values for auxilary switches
    ch[4].setpoint = ch[4].pulse;
    ch[5].setpoint = ch[5].pulse;
}

void angle_calc()
{
    short accel_data[3];
    short gyro_data[3];
    float accel_angle[3];
    float gyro_angle[3];

    // read accelerometer (X,Y,Z) values
    accel_data[X] = (i2c_read(ACCEL_XOUT_H) << 8) | i2c_read(ACCEL_XOUT_L);
    accel_data[Y] = (i2c_read(ACCEL_YOUT_H) << 8) | i2c_read(ACCEL_YOUT_L);
    accel_data[Z] = (i2c_read(ACCEL_ZOUT_H) << 8) | i2c_read(ACCEL_ZOUT_L);

    // read gyroscope (X,Y,Z) values
    gyro_data[X] = ((i2c_read(GYRO_XOUT_H) << 8) | i2c_read(GYRO_XOUT_L)) - offset[X];
    gyro_data[Y] = ((i2c_read(GYRO_YOUT_H) << 8) | i2c_read(GYRO_YOUT_L)) - offset[Y];
    gyro_data[Z] = ((i2c_read(GYRO_ZOUT_H) << 8) | i2c_read(GYRO_ZOUT_L)) - offset[Z];

    // calculate angle based on accelerometer data
    accel_angle[ROLL] = (atan2(accel_data[Y],sqrt((accel_data[X] * accel_data[X]) + (accel_data[Z] * accel_data[Z]))) * 180 / M_PI) - ROLL_OFFSET;
    accel_angle[PITCH] = (atan2(accel_data[X],sqrt((accel_data[Y] * accel_data[Y]) + (accel_data[Z] * accel_data[Z]))) * 180 / M_PI * -1) - PITCH_OFFSET;

    // calculate angle based on gyroscope data
    gyro_angle[ROLL] = angle[ROLL] + (gyro_data[X] / GYRO_SCALE / IMU_RATE);
    gyro_angle[PITCH] = angle[PITCH] + (gyro_data[Y] / GYRO_SCALE / IMU_RATE);
    gyro_angle[YAW] = gyro_data[Z] / GYRO_SCALE * -1;

    // couple roll and pitch using yaw
    gyro_angle[ROLL] -= gyro_angle[PITCH] * sin(gyro_angle[YAW] / IMU_RATE * M_PI / 180);
    gyro_angle[PITCH] += gyro_angle[ROLL] * sin(gyro_angle[YAW] / IMU_RATE * M_PI / 180);

    // check for receiver enable
    if(ch[4].setpoint > RC_MID)
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

void pid_calc()
{
    float error[3];
    float p[3];
    float i[3];
    float d[3];

    if(ch[4].setpoint > RC_MID)
    {
        // calculate error signals
        error[ROLL] = ch[0].setpoint - angle[ROLL];
        error[PITCH] = ch[1].setpoint - angle[PITCH];
        error[YAW] = ch[3].setpoint - angle[YAW];

        // proportional controller calculations
        p[ROLL] = error[ROLL] * P_ROLL_GAIN;
        p[PITCH] = error[PITCH] * P_PITCH_GAIN;
        p[YAW] = error[YAW] * P_YAW_GAIN;

        // accumulate error and saturate
        sum[ROLL] = saturate(sum[ROLL] + error[ROLL] / IMU_RATE,MAX_ANGLE * -1,MAX_ANGLE);
        sum[PITCH] = saturate(sum[PITCH] + error[PITCH] / IMU_RATE,MAX_ANGLE * -1,MAX_ANGLE);
        sum[YAW] = saturate(sum[YAW] + error[YAW] / IMU_RATE,MAX_ANGLE * -1,MAX_ANGLE);

        // integral controller calculations
        i[ROLL] = sum[ROLL] * I_ROLL_GAIN;
        i[PITCH] = sum[PITCH] * I_PITCH_GAIN;
        i[YAW] = sum[YAW] * I_YAW_GAIN;

        // derivative controller calculations
        d[ROLL] = (error[ROLL] - prev[ROLL]) * IMU_RATE * D_ROLL_GAIN;
        d[PITCH] = (error[PITCH] - prev[PITCH]) * IMU_RATE * D_PITCH_GAIN;
        d[YAW] = (error[YAW] - prev[YAW]) * IMU_RATE * D_YAW_GAIN;

        // save previous error values
        prev[ROLL] = error[ROLL];
        prev[PITCH] = error[PITCH];
        prev[YAW] = error[YAW];

        // calculate PID outputs and saturate
        pid[ROLL] = saturate(p[ROLL] + i[ROLL] + d[ROLL],MAX_ANGLE * -1,MAX_ANGLE);
        pid[PITCH] = saturate(p[PITCH] + i[PITCH] + d[PITCH],MAX_ANGLE * -1,MAX_ANGLE);
        pid[YAW] = saturate(p[YAW] + i[YAW] + d[YAW],MAX_ANGLE * -1,MAX_ANGLE);

        // linearize PID outputs to match ESC inputs
        pid[ROLL] = pid[ROLL] * OUTPUT_SCALE / MAX_ANGLE;
        pid[PITCH] = pid[PITCH] * OUTPUT_SCALE / MAX_ANGLE;
        pid[YAW] = pid[YAW] * OUTPUT_SCALE / MAX_ANGLE;
    }
    else
    {
        // initialize accumulated and previous errors
        sum[ROLL] = 0;
        sum[PITCH] = 0;
        sum[YAW] = 0;
        prev[ROLL] = 0;
        prev[PITCH] = 0;
        prev[YAW] = 0;
    }
}

void output_calc()
{
    // use throttle as base input
    esc[RF] = ch[2].setpoint - pid[ROLL] - pid[PITCH] + pid[YAW];
    esc[LF] = ch[2].setpoint + pid[ROLL] - pid[PITCH] - pid[YAW];
    esc[LB] = ch[2].setpoint + pid[ROLL] + pid[PITCH] + pid[YAW];
    esc[RB] = ch[2].setpoint - pid[ROLL] + pid[PITCH] - pid[YAW];
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// -- MAIN PROGRAM ------------------------------------------------------------
// ----------------------------------------------------------------------------
void main()
{
    int i;

    // disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize interrupt flags
    i2c_flag = 0;
    sample_flag = 0;

    // initialize offsets to 0
    offset[X] = 0;
    offset[Y] = 0;
    offset[Z] = 0;

    // initialize channel states to low
    ch[0].state = 0;
    ch[1].state = 0;
    ch[2].state = 0;
    ch[3].state = 0;
    ch[4].state = 0;
    ch[5].state = 0;

    // initialize components
    init_dco();
    init_led();
    init_esc();
    init_rc();
    init_imu();
    //init_battery();

    // enable all interrupts
    __enable_irq();

    // setup IMU configuration registers
    i2c_write(PWR_MGMT_1,PWR_MGMT_1_CLKSEL_1);
    i2c_write(CONFIG,CONFIG_DLPF_CFG_3);
    i2c_write(ACCEL_CONFIG,ACCEL_CONFIG_AFS_SEL_2);
    i2c_write(GYRO_CONFIG,GYRO_CONFIG_FS_SEL_1);

    // wait for IMU to finish configuration
    delay_ms(100);

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

    // start TimerA
    TIMER_A1->CTL |= TIMER_A_CTL_MC__UP;

    while(1)
    {
        while(!sample_flag);
        sample_flag = 0;

        input_calc();
        angle_calc();
        pid_calc();
        output_calc();

        if(ch[4].setpoint > RC_MID)
        {
            // operate motors between idle and max RPM
            TIMER_A0->CCR[RF+1] = saturate(esc[RF],ESC_IDLE,ESC_MAX);
            TIMER_A0->CCR[LF+1] = saturate(esc[LF],ESC_IDLE,ESC_MAX);
            TIMER_A0->CCR[LB+1] = saturate(esc[LB],ESC_IDLE,ESC_MAX);
            TIMER_A0->CCR[RB+1] = saturate(esc[RB],ESC_IDLE,ESC_MAX);

            // indicate drone is armed
            reset_led();
            green_led();
        }
        else
        {
            // keep motors off
            TIMER_A0->CCR[RF+1] = ESC_MIN;
            TIMER_A0->CCR[LF+1] = ESC_MIN;
            TIMER_A0->CCR[LB+1] = ESC_MIN;
            TIMER_A0->CCR[RB+1] = ESC_MIN;

            // indicate drone is disarmed
            reset_led();
            blue_led();
        }
    }
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// -- INTERRUPT SERVICE ROUTINES ----------------------------------------------
// ----------------------------------------------------------------------------
void PORT3_IRQHandler()
{
    // channel 1
    if(RC_CTRL->IFG & BIT0)
    {
        if(!ch[0].state && (RC_CTRL->IN & BIT0))
        {
            TIMER32_1->LOAD = UINT32_MAX;
            ch[0].time = TIMER32_1->VALUE;
            ch[0].state = 1;
            RC_CTRL->IES |= BIT0;
        }
        else if(ch[0].state && !(RC_CTRL->IN & BIT0))
        {
            ch[0].pulse = ch[0].time - TIMER32_1->VALUE;
            ch[0].state = 0;
            RC_CTRL->IES &= ~BIT0;
        }

        RC_CTRL->IFG &= ~BIT0;
    }

    // channel 2
    if(RC_CTRL->IFG & BIT2)
    {
        if(!ch[1].state && (RC_CTRL->IN & BIT2))
        {
            ch[1].time = TIMER32_1->VALUE;
            ch[1].state = 1;
            RC_CTRL->IES |= BIT2;
        }
        else if(ch[1].state && !(RC_CTRL->IN & BIT2))
        {
            ch[1].pulse = ch[1].time - TIMER32_1->VALUE;
            ch[1].state = 0;
            RC_CTRL->IES &= ~BIT2;
        }

        RC_CTRL->IFG &= ~BIT2;
    }

    // channel 3
    if(RC_CTRL->IFG & BIT3)
    {
        if(!ch[2].state && (RC_CTRL->IN & BIT3))
        {
            ch[2].time = TIMER32_1->VALUE;
            ch[2].state = 1;
            RC_CTRL->IES |= BIT3;
        }
        else if(ch[2].state && !(RC_CTRL->IN & BIT3))
        {
            ch[2].pulse = ch[2].time - TIMER32_1->VALUE;
            ch[2].state = 0;
            RC_CTRL->IES &= ~BIT3;
        }

        RC_CTRL->IFG &= ~BIT3;
    }

    // channel 4
    if(RC_CTRL->IFG & BIT5)
    {
        if(!ch[3].state && (RC_CTRL->IN & BIT5))
        {
            ch[3].time = TIMER32_1->VALUE;
            ch[3].state = 1;
            RC_CTRL->IES |= BIT5;
        }
        else if(ch[3].state && !(RC_CTRL->IN & BIT5))
        {
            ch[3].pulse = ch[3].time - TIMER32_1->VALUE;
            ch[3].state = 0;
            RC_CTRL->IES &= ~BIT5;
        }

        RC_CTRL->IFG &= ~BIT5;
    }

    // channel 5
    if(RC_CTRL->IFG & BIT6)
    {
        if(!ch[4].state && (RC_CTRL->IN & BIT6))
        {
            ch[4].time = TIMER32_1->VALUE;
            ch[4].state = 1;
            RC_CTRL->IES |= BIT6;
        }
        else if(ch[4].state && !(RC_CTRL->IN & BIT6))
        {
            ch[4].pulse = ch[4].time - TIMER32_1->VALUE;
            ch[4].state = 0;
            RC_CTRL->IES &= ~BIT6;
        }

        RC_CTRL->IFG &= ~BIT6;
    }

    // channel 6
    if(RC_CTRL->IFG & BIT7)
    {
        if(!ch[5].state && (RC_CTRL->IN & BIT7))
        {
            ch[5].time = TIMER32_1->VALUE;
            ch[5].state = 1;
            RC_CTRL->IES |= BIT7;
        }
        else if(ch[5].state && !(RC_CTRL->IN & BIT7))
        {
            ch[5].pulse = ch[5].time - TIMER32_1->VALUE;
            ch[5].state = 0;
            RC_CTRL->IES &= ~BIT7;
        }

        RC_CTRL->IFG &= ~BIT7;
    }
}

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

void TA1_0_IRQHandler()
{
    // set sample flag and clear interrupt flag
    sample_flag = 1;
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

void ADC14_IRQHandler()
{
    if(((ADC14->MEM[0] * BATTERY_DIVIDER * VDD / SCALE) < BATTERY_THRESHOLD) && (ch[5].setpoint > RC_MID))
    {
        // start Timer32
        TIMER32_2->CONTROL |= TIMER32_CONTROL_ENABLE;
    }
    else
    {
        // turn off buzzer and stop Timer32
        BATTERY_CTRL->OUT &= ~BIT1;
        TIMER32_2->CONTROL &= ~TIMER32_CONTROL_ENABLE;
    }

    // start ADC14 sampling
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
}

void T32_INT2_IRQHandler()
{
    //red_led(); //???

    // toggle buzzer and clear Timer32 interrupt
    BATTERY_CTRL->OUT ^= BIT1;
    TIMER32_2->INTCLR++;
}
// ----------------------------------------------------------------------------
