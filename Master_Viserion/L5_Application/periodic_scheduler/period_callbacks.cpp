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


#include <CommonHeader/commonVariables.h>
#include <Periodic_Hearbeats/receive_heartbeats.h>
#include <Periodic_Hearbeats/send_heartbeats.h>
//#include "Sensor_Values_Received/receive_sensor_values.h"
#include "Geo_Values_Received/geo_find_directions.h"
#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "io.hpp"
#include "Transmit_Data_To_Motor/transmit_sensor_to_motor.h"
#include<string>
#include<stdio.h>

bool wait_for_start(can_msg_t *crx,dbc_msg_hdr_t *rx);
bool send_start_signal();
bool send_stop();
//bool take_decision(can_msg_t *crx,dbc_msg_hdr_t *rx);

//#include "_can_dbc/generated_Viserion.h"



const can_t canbusno = can1;
/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 8);


bool gChangeState = false;
state_machine gCurrentState = init_car;

//unsigned int gSpeed = brake;
//unsigned int gDirection = straight;

unsigned int gGeoSpeed = brake;
unsigned int gGeoDirection = straight;

//unsigned int gGeo_Speed = brake;
//unsigned int gGeo_Direction = straight;

static const char* enum_strings[]={"init_car","send_start","get_distance_heading","get_sensor_vals","stop_car","abort_mission","gps_no_lock"};

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    CAN_init(canbusno,100,100,100,NULL,NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(canbusno);

/*
    gChangeState = false;
    gCurrentState = init_car;
*/


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
    if(CAN_is_bus_off(canbusno))
    {
#ifdef DEBUG_PRINT
        printf("reset\n");
#endif
        CAN_reset_bus(canbusno);
        LE.toggle(1); //to check can bus off
    }
    //receive_heartbeats(canbusno,1000);
#ifdef DEBUG_PRINT
    if(count%10==0)
    {
        printf("drop=%d\n",CAN_get_rx_dropped_count(can1));
    }
#endif


}

void period_10Hz(uint32_t count)
{

}

void period_100Hz(uint32_t count)
{
    can_msg_t can_msg = {0};
    dbc_msg_hdr_t msgRx = {0};
    if(CAN_rx(can1, &can_msg, 0))
    {
        if(can_msg.msg_id == 405 || can_msg.msg_id == 84)
        {
            gCurrentState= stop_car;
            gChangeState = false;
        }
#ifdef DEBUG_PRINT
        printf("s=%s\n",enum_strings[gCurrentState]);
#endif
        switch(gCurrentState)
        {
            case init_car:
                gChangeState = false;
                wait_for_start(&can_msg,&msgRx);
                if(gChangeState)
                {
                    gCurrentState = send_start;
                }
                break;
            case send_start:
                gChangeState = false;
                send_start_signal();
                if(gChangeState)
                {
                    gCurrentState = get_distance_heading;
                }
                break;
            case get_distance_heading:
                gChangeState = false;
                take_decision(&can_msg,&msgRx, gGeoSpeed, gGeoDirection); //references
               // take_decision(&can_msg,&msgRx);
                if(gChangeState)
                {
#ifdef DEBUG_PRINT
                    printf("sp=%d dir=%d\n",gGeoSpeed, gGeoDirection);
#endif
                    gCurrentState = get_sensor_vals;
                }
                break;
            case get_sensor_vals:
                gChangeState = false;
                receiveSensorValues(gGeoSpeed,gGeoDirection,&can_msg,&msgRx);
                if(gChangeState)
                {
                    gCurrentState = get_distance_heading;
                }
                break;
            case stop_car:
                gChangeState = false;
                transmit_to_motor(brake,straight);
                send_stop();
                if(gChangeState)
                {
                    gCurrentState = init_car;
                }
                break;
            case abort_mission:
                break;
            case gps_no_lock:
                break;
        }
    }

/*
    can_msg_t can_msg = {0};
    dbc_msg_hdr_t msgRx = {0};
    if(CAN_rx(can1, &can_msg, 0))
    {
        msgRx.dlc = can_msg.frame_fields.data_len;
        msgRx.mid = can_msg.msg_id;
        switch(msgRx.mid)
        {
            case Sensor_Data_Id :
                dbc_decode_SENSORS_VALUES(&sensor_st, can_msg.data.bytes, &msgRx);
                LE.set(2, 1);
                checkSensorValues();
                break;
        }
    }

*/



   // receiveSensorValues();
    // LE.toggle(3);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
   // LE.toggle(4);
}



bool wait_for_start(can_msg_t *crx,dbc_msg_hdr_t *rx)
{
    if(crx->msg_id == 83)
    {
        gChangeState = true;
    }
    return true;
}

bool send_start_signal()
{
    /*
     *
     * dbc encode sample
    MASTER_SPEED_t motor_msg;
    motor_msg.MASTER_Maintain_Speed = speed;
    motor_msg.MASTER_Maintain_Direction = direction;
    can_msg_t can_motor = {0};
    dbc_msg_hdr_t msg_hdr = dbc_encode_MASTER_SPEED(can_motor.data.bytes, &motor_msg);
    can_motor.msg_id = msg_hdr.mid;
    can_motor.frame_fields.data_len = msg_hdr.dlc;

    return CAN_tx(can1, &can_motor, 0);
    */

    MASTER_START_CMD_t start_msg;
    start_msg.CAR_INIT =1; //Dont care
    can_msg_t can_start = {0};


    dbc_msg_hdr_t msg_hdr = dbc_encode_MASTER_START_CMD(can_start.data.bytes,&start_msg);

    can_start.msg_id = msg_hdr.mid;
    can_start.frame_fields.data_len = msg_hdr.dlc;
    if(CAN_tx(can1,&can_start,0))
    {
        gChangeState = true;
    }

    return true;
}

bool send_stop()
{
    MASTER_STOP_CMD_t stop_msg;
    stop_msg.CAR_HALT =1;
    can_msg_t can_stop={0};
    dbc_msg_hdr_t msg_hdr_stop= dbc_encode_MASTER_STOP_CMD(can_stop.data.bytes,&stop_msg);

    can_stop.msg_id=msg_hdr_stop.mid;
    can_stop.frame_fields.data_len= msg_hdr_stop.dlc;

    if(CAN_tx(can1,&can_stop,0))
    {
        gChangeState =true;
    }

    return true;
}
