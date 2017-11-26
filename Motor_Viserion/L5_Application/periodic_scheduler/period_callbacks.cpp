/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * This contains the period callback functions for the periodic scheduler
 *
 * @warning
 * These callbacks should be used for hard real-time system, and the priority of these
 * tasks are above everything else in the system (above the PRIORITY_CRITICAL).
 * The period functions SHOULD NEVER block and SHOULD NEVER run over their time slot.
 * For example, the 1000Hz take slot runs periodically every 1ms, and whatever you
 * do must be completed within 1ms.  Running over the time slot will reset the system.
 */

#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "can.h"
#include <stdio.h>
#include "_can_dbc/generated_Viserion.h"
#include <cstring>

#include "dbc_app_send_can_msg.h"
#include "send_motor_heartbeat.h"
#include "receive_heartbeats.h"
#include "lpc_pwm.hpp"
#include "eint.h"
#include "gpio.hpp"
#include "eint.h"
#include "printf_lib.h"
#include "storage.hpp"
#include "uart3.hpp"


MASTER_SPEED_t speed = { 0 };

//MIA values
extern const uint32_t MASTER_SPEED__MIA_MS = 10000;
extern const MASTER_SPEED_t MASTER_SPEED__MIA_MSG = { 1, 0 };

MOTOR_STATUS_t sending_speed_kph = { 1 };

//PWM for dc motor (speed) and servo(dir)
PWM motor_speed(PWM::pwm1, 10);
PWM motor_dir(PWM::pwm2, 10);

Uart3& u3 = Uart3::getInstance();

#define MOTOR_INIT          18.6
#define FORWARD_SLOW        19
#define FORWARD_MEDIUM      19.2    //19.2
#define FORWARD_FAST        20.5
#define REVERSE             16.8
#define BRAKES              12

#define SERVO_CENTER        17.7
#define SERVO_HARD_LEFT     10.9
#define SERVO_SLIGHT_LEFT   14.9
#define SERVO_HARD_RIGHT    25
#define SERVO_SLIGHT_RIGHT  20.7

#define GEAR_RATIO 					    12.58			//Gear to Wheel Ratio: Wheel rotates 1x for every 12.58 revolution of the DC motor
#define PI								3.14159
#define WHEEL_RADIUS					2.794			//Unit is centimeters
#define CONST_KPH                       0.036

#define STOP_SPEED                      1
#define SLOW_SPEED                      7 //8
#define MEDIUM_SPEED                    10 //8

uint32_t no_of_revolution = 0;
float value, kph = 0;
int speed_req = 0;
int count_speed = 0;
int rps = 0;

int stop_flag = 0;

float compare_pwm = 0;
volatile uint8_t speed_calc = 1;
int count_10, count_30, count_50, count_100;
int count_15, count_16, count_17, count_18, count_23, count_28;
bool write_flag = false;

uint8_t speed_curr = 0;
uint8_t speed_prev = 10;

uint8_t dir_curr = 0;
uint8_t dir_prev = 10;

uint32_t curr_rps = 0;
uint32_t req_rps = 0;

//Default values
static float motor_speed_val = 18, servo_dir_val = 18;
static int reverse_count = 0;

//RPM sensor
GPIO rpm_pin(P2_6);

//logging to sd card buffer
char buffer_file[150] = { 0 };

int16_t tilt_x, tilt_y;
int length = 0;

int n = 0;
bool flag_checker = false;
bool reverse_flag = false;
bool motor_init_flag = false;
bool brake_flag = 0;

typedef enum {
    uphill, downhill, invalid
} orientation_t;

orientation_t orient = invalid;

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);

void reverse_init()
{
    if (reverse_count == 5) motor_speed.set(MOTOR_INIT); //motor_speed_val = MOTOR_INIT;
    else if (reverse_count == 10) motor_speed.set(REVERSE); //motor_speed_val = REVERSE;
    else if (reverse_count == 15) {
        motor_speed.set(MOTOR_INIT);
        reverse_count = 20;
    }

}

/// Called once before the RTOS is started, this is a good place to initialize things once
void count_rpm()
{
    no_of_revolution++;
}

void bus_off(uint32_t)
{
    CAN_reset_bus(can1);
}

void data_ovr(uint32_t)
{
    CAN_reset_bus(can1);
}

bool period_init(void)
{
    //Configure CAN1
    CAN_init(can1, 100, 100, 100, bus_off, data_ovr);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);

    //Initialize motor and servo
    motor_speed.set(18);
    motor_dir.set(18);

    //Set GPIO as interrupt pin connected to RPM sensor
    rpm_pin.setAsInput();
    eint3_enable_port2(6, eint_falling_edge, count_rpm);

    //Bluetooth
   // u3.init(9600);

    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}

/**
 * Below are your periodic functions.
 * The argument 'count' is the number of times each periodic task is called.
 */


void period_1Hz(uint32_t count)
{
    //Calculate speed
    kph = (2 * PI * WHEEL_RADIUS * CONST_KPH * no_of_revolution);

    curr_rps = no_of_revolution;
/* This loop checks for the difference in the current and required RPS and then
 * modifies PWM to maintain the desired speed.
 */
#if 1
        /*if(req_rps==0){
            motor_speed.set(BRAKES);
            motor_speed_val=MOTOR_INIT;
        }
        else{*/
        if(req_rps!=0){
        if (orient == invalid) {
            int diff = req_rps - curr_rps;
            if (diff > 5) motor_speed_val += 0.3;
            //if (diff > 3.5 && diff <= 5) motor_speed_val += 0.1;
            else if (diff > 3.5) motor_speed_val += 0.1;
            else if (diff > 1 && diff <= 3.5) motor_speed_val += 0.01;
            else if (diff < 0 && diff >= -2) motor_speed_val -= 0.01;
            else if (diff < (-2)) motor_speed_val -= 0.05;
        }
        else if (orient == uphill) {
            int diff = req_rps - curr_rps;
            if (diff > 5) motor_speed_val = 19.2;
            else if (diff > 3.5 && diff <= 5) motor_speed_val += 0.25;
            else if (diff > 1 && diff <= 3.5) motor_speed_val += 0.01;
            else if (diff < 0 && diff >= -2) motor_speed_val -= 0.01;
            else if (diff < (-2)) motor_speed_val -= 0.05;
        }
        else if (orient == downhill) {
            int diff = curr_rps - req_rps;
            if (diff > 8) motor_speed_val = 18.3;
            else if (diff > 4) motor_speed_val = 18.7;
            else if (diff > 3.5) motor_speed_val -= 0.3;
            else if (diff > 1) motor_speed_val -= 0.01;
            else if (diff < 0) motor_speed_val += 0.005;
        }
        motor_speed.set(motor_speed_val);}
#endif
    if (CAN_is_bus_off(can1)) CAN_reset_bus(can1);

    //Heartbeats
    send_motor_heartbeat();
    //receive_heartbeats();

    //Send speed to Master

/*
    sending_speed_kph.MOTOR_Send_PWM = motor_speed_val;
    sending_speed_kph.MOTOR_Send_Speed = no_of_revolution;
    dbc_encode_and_send_MOTOR_STATUS(&sending_speed_kph);
*/
    LD.setNumber(no_of_revolution);
    no_of_revolution = 0;
}
void period_10Hz(uint32_t count)
{
    //Calculate speed
    /*    no_of_revolution*=10;
     kph = (2 * PI * WHEEL_RADIUS * CONST_KPH * no_of_revolution);

     //Send speed to Master
     sending_speed_kph.MOTOR_Send_Speed=no_of_revolution;
     dbc_encode_and_send_MOTOR_STATUS(&sending_speed_kph);

     curr_rps=no_of_revolution;
     no_of_revolution = 0;*/

#if 0
        if (orient == invalid) {
            int diff = req_rps - curr_rps;
            if (diff > 5) motor_speed_val += 0.04;
            else if (diff > 3.5 && diff <= 5) motor_speed_val += 0.03;
            else if (diff > 1 && diff <= 3.5) motor_speed_val += 0.001;
            else if (diff < 0 && diff >= -2) motor_speed_val -= 0.001;
            else if (diff < (-2)) motor_speed_val -= 0.05;
        }
        else if (orient == uphill) {
            int diff = req_rps - curr_rps;
            if (diff > 5) motor_speed_val = 19.3;
            else if (diff > 3.5 && diff <= 5) motor_speed_val += 0.03;
            else if (diff > 1 && diff <= 3.5) motor_speed_val += 0.001;
            else if (diff < 0 && diff >= -2) motor_speed_val -= 0.001;
            else if (diff < (-2)) motor_speed_val -= 0.005;
        }
        else if (orient == downhill) {
            int diff = curr_rps - req_rps;
            if (diff > 8) motor_speed_val = 18.3;
            else if (diff > 4) motor_speed_val = 18.7;
            else if (diff > 3.5) motor_speed_val -= 0.03;
            else if (diff > 1) motor_speed_val -= 0.001;
            else if (diff < 0) motor_speed_val += 0.0005;
        }
        motor_speed.set(motor_speed_val);
#endif

    tilt_x = AS.getX();
    tilt_y=AS.getY();
    if(tilt_x>46) {  //&& tilt_x>tilt_y  //old value - x>76
        //uphill
        LE.off(1);
        LE.off(2);
        orient=uphill;
        LE.toggle(4);
    }
    else if(tilt_x<(-150)) { //&& tilt_x<tilt_y) //old val x<-150
        //downhill
        LE.off(4);
        LE.off(2);
        orient=downhill;
        LE.toggle(1);
    }
    else {
        //invalid
        LE.off(1);
        LE.off(4);
        orient=invalid;
        LE.toggle(2);
    }

    //To test dc motor working based on switch input
    if(SW.getSwitch(1))
    motor_speed_val=18;
    else if(SW.getSwitch(2))
    motor_speed_val=19.3;
    else if(SW.getSwitch(3))
    motor_speed_val+=0.1;
    else if(SW.getSwitch(4))
    motor_speed_val-=0.1;

    //LD.setNumber(motor_speed_val);

}

void period_100Hz(uint32_t count)
{

#if 1
    can_msg_t motor_message;
    dbc_msg_hdr_t can_msg_hdr;

    while (CAN_rx(can1, &motor_message, 0)) {

        can_msg_hdr.dlc = motor_message.frame_fields.data_len;
        can_msg_hdr.mid = motor_message.msg_id;

        if (motor_message.msg_id == 203) {
            dbc_decode_MASTER_SPEED(&speed, motor_message.data.bytes, &can_msg_hdr);

            speed_curr = speed.MASTER_Maintain_Speed;

            if (speed_prev != speed_curr) {
                /*if (brake_flag) {
                    motor_speed_val = MOTOR_INIT;
                    brake_flag = false;
                }*/
                switch (speed_curr) {
                    case 0:
                        //reverse
                        break;
                    case 1:
                        //brakes
                        //motor_speed_val = MOTOR_INIT;
                        //motor_speed.set(BRAKES);
                        req_rps = 0;
                        brake_flag=true;
                        break;
                    case 2:
                        //forward slow
                        req_rps = 6;
                        break;
                    case 3:
                        //fast
                        req_rps = 6;
                        break;
                    case 4:
                        //turbo
                        break;
                    case 5:
                        //do nothing
                        break;
                    default:
                        break;
                        //MIA
                }
                speed_prev = speed_curr;
            }

            dir_curr = speed.MASTER_Maintain_Direction;
            if (dir_curr != dir_prev) {
                switch (dir_curr) {
                    case 0:
                        //center
                        servo_dir_val = SERVO_CENTER;
                        break;
                    case 1:
                        //hard left 10.9
                        servo_dir_val = SERVO_HARD_LEFT;
                        //servo_dir_val = SERVO_SLIGHT_LEFT;
                        break;
                    case 2:
                        //slight-left 14.9
                        servo_dir_val = SERVO_SLIGHT_LEFT;
                        break;
                    case 3:
                        //hard right 25
                        servo_dir_val = SERVO_HARD_RIGHT;
                        //servo_dir_val = SERVO_SLIGHT_RIGHT;
                        break;
                    case 4:
                        //slight right 20.7
                        servo_dir_val = SERVO_SLIGHT_RIGHT;
                        break;
                }
                dir_prev = dir_curr;
            }
        }
    }

    if (dbc_handle_mia_MASTER_SPEED(&speed, 250)) {
        LD.setNumber(99);
    }
#endif

    //Set servo direction based on Master command
    motor_dir.set(servo_dir_val);

    if (req_rps == 0) {
        motor_speed.set(BRAKES);
        motor_speed_val = MOTOR_INIT;
    }

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));

/*
 * Initial case : diff = 0, do nothing
 * first time: diff>5, so increment from 18 to 18.5
 * */
void period_1000Hz(uint32_t count)
{
#if 0
    if (orient == invalid) {
        int diff = req_rps - curr_rps;
        if (diff > 5) motor_speed_val += 0.0005;
        else if (diff > 3.5 && diff<=5) motor_speed_val += 0.0003;
        else if (diff > 1 && diff <=3.5) motor_speed_val += 0.0001;
        else if (diff < 0 && diff <=1) motor_speed_val -= 0.0001;
    }
    else if (orient == uphill) {
        int diff = req_rps - curr_rps;
        if (diff > 5) motor_speed_val = 19.3;
        else if (diff > 3.5 && diff<=5) motor_speed_val += 0.0003;
        else if (diff > 1 && diff <=3.5) motor_speed_val += 0.0001;
        else if (diff < 0 && diff <=1) motor_speed_val -= 0.0001;
    }
    else if (orient == downhill) {
        int diff = curr_rps - req_rps;
        if (diff > 7) motor_speed_val = 18.8;
        else if (diff > 3.5) motor_speed_val -= 0.0003;
        else if (diff > 1) motor_speed_val -= 0.0001;
        else if (diff < 0) motor_speed_val += 0.0001;
    }
#endif
}

void logSpeed(float text)
{

    float buffer = text;
    Storage::append("1:logf.txt", &buffer, 1, 0);

}
