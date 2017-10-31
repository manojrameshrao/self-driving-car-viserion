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

MASTER_SPEED_t speed = { 0 };

PWM motor_speed(PWM::pwm1, 10);
PWM motor_dir(PWM::pwm2, 10);

//Default values
static float motor_speed_val = 12, servo_dir_val=18;

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
    CAN_init(can1, 100, 100, 100, bus_off, data_ovr);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);
    motor_speed.set(19.7);
    motor_dir.set(18);
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
    if (CAN_is_bus_off(can1)) CAN_reset_bus(can1);

    //motor_speed.set(19.5);
    //LE.on(3);
    //send_motor_heartbeat();
    //receive_heartbeats();
}

void period_10Hz(uint32_t count)
{

    // LE.toggle(2);
}

void period_100Hz(uint32_t count)
{
    can_msg_t motor_message;
    dbc_msg_hdr_t can_msg_hdr;

    while (CAN_rx(can1, &motor_message, 0)) {

        can_msg_hdr.dlc = motor_message.frame_fields.data_len;
        can_msg_hdr.mid = motor_message.msg_id;

        if (motor_message.msg_id == 203) {
            //dbc_decode_MASTER_DIRECTION(&dir, motor_message.data.bytes, &can_msg_hdr);
            dbc_decode_MASTER_SPEED(&speed, motor_message.data.bytes, &can_msg_hdr);
            switch (speed.MASTER_Maintain_Speed) {
                case 0:
                    //reverse
                {
                    float value3 = 0;
                    motor_speed.set(value3);
                    value3 -= 0.1;
                    if (value3 <= 16.5) value3 = 16.5;
                    motor_speed_val = value3;
                }
                    break;
                case 1:
                    //brakes
                    //motor_speed.set(12);
                    motor_speed_val = 12;
                    LE.on(1);
                    break;
                case 2:
                    //forward slow
                    //motor_speed.set(19.7);
                    motor_speed_val = 19.7;
                    break;
                case 3:
                    //fast
                    //motor_speed.set(20);
                    motor_speed_val = 20;
                    break;
                case 4:
                    //turbo
                    break;
                default:
                    break;
                    //MIA
                }

            switch (speed.MASTER_Maintain_Direction) {
                case 0:
                    //center
                    //motor_dir.set(18);
                    servo_dir_val = 18;
                    break;
                case 1:
                    //hard left 10.9
                    //motor_dir.set(10.9);
                    servo_dir_val = 10.9;
                    break;
                case 2:
                    //slight-left 14.9
                    //motor_dir.set(14.9);
                    servo_dir_val = 14.9;
                    break;
                case 3:
                    //hard right 25
                    //motor_dir.set(25);
                    servo_dir_val = 25;
                    break;
                case 4:
                    //slight right 20.7
                    //motor_dir.set(20.7);
                    servo_dir_val = 20.7;
                    break;
            }
        }

    }

    motor_speed.set(motor_speed_val);
    motor_dir.set(servo_dir_val);

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
    //LE.toggle(4);
}
