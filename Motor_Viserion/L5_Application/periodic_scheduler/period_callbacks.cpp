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
#include <string.h>

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

MOTOR_STATUS_t motor_send_speed={1};

PWM motor_speed(PWM::pwm1, 10);
PWM motor_dir(PWM::pwm2, 10);

Uart3& u3 = Uart3::getInstance();

const float MOTOR_INIT = 18;
#define FORWARD_SLOW        18.8
#define FORWARD_MEDIUM      19    //19.2
#define FORWARD_FAST        20.5
#define REVERSE             16.3
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
#define SLOW_SPEED                      8

uint32_t no_of_revolution = 0;
float value, kph = 0;
int speed_req = 0;
int count_speed = 0;
int rps = 0;

int stop_flag = 0;

float compare_pwm=0;

int count_10, count_30, count_50, count_100;

uint8_t speed_curr=0;
uint8_t speed_prev=10;

uint8_t dir_curr=0;
uint8_t dir_prev=10;

//Default values
static float motor_speed_val = MOTOR_INIT, servo_dir_val = 18;
static int count_rps, reverse_count = 0;
GPIO rpm_pin(P0_29);
char buffer_file[50];

char num[5] = { 0 };

bool flag_checker = false;

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);

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
    motor_speed.set(MOTOR_INIT);
    motor_dir.set(18);

    //Set GPIO as interrupt pin connected to RPM sensor
    rpm_pin.setAsInput();
    eint3_enable_port0(29, eint_falling_edge, count_rpm);

    //Bluetooth
    u3.init(9600);

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

    no_of_revolution = 0;

    if (CAN_is_bus_off(can1)) CAN_reset_bus(can1);

    send_motor_heartbeat();
    receive_heartbeats();

    motor_send_speed.MOTOR_Send_Speed=kph;
    dbc_encode_and_send_MOTOR_STATUS(&motor_send_speed);

}
void period_10Hz(uint32_t count)
{

}

void period_100Hz(uint32_t count)
{
    can_msg_t motor_message;
    dbc_msg_hdr_t can_msg_hdr;


    while (CAN_rx(can1, &motor_message, 0)) {

        can_msg_hdr.dlc = motor_message.frame_fields.data_len;
        can_msg_hdr.mid = motor_message.msg_id;

        if (motor_message.msg_id == 203) {
            dbc_decode_MASTER_SPEED(&speed, motor_message.data.bytes, &can_msg_hdr);

            speed_curr = speed.MASTER_Maintain_Speed;

            if(speed_prev!=speed_curr){
            switch (speed_curr) {
                case 0:
                    //reverse
                    if (reverse_count == 5) compare_pwm = MOTOR_INIT;
                    else if (reverse_count == 10) {
                        compare_pwm = REVERSE;
                        reverse_count = 0;
                    }
                    reverse_count++;
                    break;
                case 1:
                    //brakes
                    speed_req = STOP_SPEED;
                    compare_pwm = BRAKES;
                    motor_speed.set(compare_pwm);
                    LE.on(4);
                    break;
                case 2:
                    //forward slow
                    compare_pwm = FORWARD_SLOW;
                    speed_req = SLOW_SPEED-1;
                    //    LE.off(1);
                    break;
                case 3:
                    //fast
                    compare_pwm = FORWARD_MEDIUM;
                    speed_req = SLOW_SPEED;
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
            speed_prev=speed_curr;
            }

            dir_curr=speed.MASTER_Maintain_Direction;
            if(dir_curr!=dir_prev){
            switch (dir_curr) {
                case 0:
                    //center
                    servo_dir_val = SERVO_CENTER;
                    break;
                case 1:
                    //hard left 10.9
                    servo_dir_val = SERVO_HARD_LEFT;
                    break;
                case 2:
                    //slight-left 14.9
                    servo_dir_val = SERVO_SLIGHT_LEFT;
                    break;
                case 3:
                    //hard right 25
                    servo_dir_val = SERVO_HARD_RIGHT;
                    break;
                case 4:
                    //slight right 20.7
                    servo_dir_val = SERVO_SLIGHT_RIGHT;
                    break;
            }
            dir_prev=dir_curr;
            }
        }
    }

   // compare_pwm = motor_speed_val;
    motor_speed.set(motor_speed_val);
    motor_dir.set(servo_dir_val);
    LD.setNumber(kph);
    count_speed++;

/*    if(SW.getSwitch(1))
        motor_speed_val=18;
    else if(SW.getSwitch(2))
        motor_speed_val=18.5;
    else if(SW.getSwitch(3))
        motor_speed_val=19;*/

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
    if (speed_req == STOP_SPEED) {
        motor_speed_val = BRAKES;
        stop_flag=1;
    }
    else if (kph > speed_req) {
        {
            motor_speed_val -= 0.003;
            if (motor_speed_val <= compare_pwm) motor_speed_val = compare_pwm;
            count_speed = 0;
        }
    }
    else if (kph < speed_req) {
        {
            motor_speed_val += 0.001;
            if (motor_speed_val > compare_pwm) motor_speed_val = compare_pwm;
            count_speed = 0;

        }
    }

    if(stop_flag==1 && speed_req!=STOP_SPEED){
        motor_speed_val=18.7;
        stop_flag=2;
    }

}

void logSpeed(float text)
{

    float buffer = text;
    Storage::append("1:logf.txt", &buffer, 1, 0);

}
