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

const uint32_t                             HB_SENSORS__MIA_MS = 3000;
const HB_SENSORS_t                         HB_SENSORS__MIA_MSG = {0};
HB_SENSORS_t sensor_msg={0};


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
	CAN_init(can1,100,100,100,bus_off,data_ovr);
	CAN_bypass_filter_accept_all_msgs();
	CAN_reset_bus(can1);
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

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}

void motor_hb()
{
	HB_MOTORS_t motor_msg={1};
	motor_msg.MOTOR_ALIVE=1;
	dbc_encode_and_send_HB_MOTORS(&motor_msg);
}



void period_1Hz(uint32_t count)
{
	if(CAN_is_bus_off(can1))
	{
	    LE.toggle(2);
	    CAN_reset_bus(can1);
	}
    LE.toggle(1);
    motor_hb();
}

void period_10Hz(uint32_t count)
{
   // LE.toggle(2);
}

void period_100Hz(uint32_t count)
{
	can_msg_t msg;

	while(CAN_rx(can1,&msg,0))
	{
		dbc_msg_hdr_t can_msg_hdr;
	    can_msg_hdr.dlc = can_msg.frame_fields.data_len;
		can_msg_hdr.mid = can_msg.msg_id;

		if(can_msg_hdr.mid==95)
		{
			LE.toggle(3);
		}
	}

   // LE.toggle(3);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
    //LE.toggle(4);
}

if(dbc_handle_mia_HB_SENSORS(&sensor_msg, 100))
        LD.setNumber(1);

