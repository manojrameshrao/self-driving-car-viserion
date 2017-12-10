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

bool wait_for_start(can_msg_t *crx,dbc_msg_hdr_t *rx);
bool send_start_signal();
//bool take_decision(can_msg_t *crx,dbc_msg_hdr_t *rx);

//#include "_can_dbc/generated_Viserion.h"
#include "Transmit_Data_To_Motor/transmit_sensor_to_motor.h"


const can_t canbusno = can1;
/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 8);


bool gChangeState = false;
state_machine gCurrentState = init_car;


uint8_t speed= brake,direction= straight;

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
        CAN_reset_bus(canbusno);
    }
    //receive_heartbeats(canbusno,1000);

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
        switch(gCurrentState)
        {
            case init_car:
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
                take_decision(&can_msg,&msgRx);
                if(gChangeState)
                {
                    gCurrentState = get_sensor_vals;
                }
                break;
            case get_sensor_vals:
                gChangeState = false;
                receiveSensorValues(speed,direction,&can_msg,&msgRx);
                if(gChangeState)
                {
                    gCurrentState = get_distance_heading;
                }
                break;
            case stop_car:
                transmit_to_motor(brake,straight);
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


// **************************************///////////////////
// SENSOR CODE//
/*
const uint32_t                             SENSORS_VALUES__MIA_MS  = {2000};
const SENSORS_VALUES_t                     SENSORS_VALUES__MIA_MSG = {255,255,255};

SENSORS_VALUES_t sensor_st = {0};

#define CORNER_DIST 30  //25 - Previous Commit Values
#define MIDDLE_DIST 35 //25 - Previous Commit Values
#define REAR_DIST   20 // Not sure

#define MIN_MIDDLE_DIST 10
#define MIN_CORNER_DIST 15
#define MIN_REAR_DIST


bool receiveSensorValues(unsigned int sp,unsigned int dir,can_msg_t *crx,dbc_msg_hdr_t *rx)
{
    can_msg_t can_msg = {0};
    dbc_msg_hdr_t msgRx = {0};
    //if(CAN_rx(can1, &can_msg, 0))
    {
        msgRx.dlc = can_msg.frame_fields.data_len;
        msgRx.mid = can_msg.msg_id;

        rx->dlc = crx->frame_fields.data_len;
        rx->mid = crx->msg_id;

        switch(rx->mid)
        {
            case Sensor_Data_Id :
                if(dbc_decode_SENSORS_VALUES(&sensor_st, can_msg.data.bytes, &msgRx))
                {
                    LE.set(2, 1);
                    checkSensorValues(sp,dir);
                }
                break;
        }
    }
    if(dbc_handle_mia_SENSORS_VALUES(&sensor_st, 10))
    {
        transmit_to_motor(brake,straight); //should be brake
        LD.setNumber(19);
        LE.set(2, 0);
    }
    return true;
}

*/
/*
bool checkSensorValues(uint8_t sp,uint8_t dir)
{
    static uint64_t isReverse = 0;
    static uint8_t prev_speed = 0;
    bool waitReverse = false;

    //uint8_t speed = 0;
   // uint8_t direction = 0;

    if(sensor_st.SENSOR_middle_in <= MIN_MIDDLE_DIST) //if middle : 0 to 10
    {
        LD.setNumber(5);
        sp = brake; //1
        dir = straight; //0
        isReverse++;
*/      /*    if(sensor_st.SENSOR_back_in <= REAR_DIST)
        {
            waitReverse = false;
        }
        else*/
/*
        {
            waitReverse = true;
        }

    }
    else if(sensor_st.SENSOR_middle_in > MIN_MIDDLE_DIST && sensor_st.SENSOR_middle_in <= MIDDLE_DIST) //middle detected 11 - 25
    {
        if(sensor_st.SENSOR_left_in <= CORNER_DIST && sensor_st.SENSOR_right_in <= CORNER_DIST )
        {
            //left(0-25) + middle(11 - 25) + right(0-25)
            LD.setNumber(60);
            sp = brake; // 1
            dir = straight; //0
            isReverse++;
            if(sensor_st.SENSOR_back_in <= REAR_DIST)
            {
                waitReverse = false;
            }
            else
            {
                waitReverse = true;
            }

        }
        else if(sensor_st.SENSOR_right_in <= CORNER_DIST) // right(0 - 25) + middle(11 - 25)
        {
            LD.setNumber(70);
            sp = slow; //2
            dir = full_left; //1
        }
        else if(sensor_st.SENSOR_left_in <= CORNER_DIST)  //left(0 - 25) + middle (11 -25)
        {
            LD.setNumber(50);
            sp = slow; //2
            dir = full_right; //3
        }
        else //middle(11 - 25)
        {
            LD.setNumber(20);
            sp = slow; //slow, 2
            dir = slight_left; //2
        }
    }
    else // (middle > 25)
    {
       if(sensor_st.SENSOR_left_in <= MIN_CORNER_DIST && sensor_st.SENSOR_right_in <= MIN_CORNER_DIST)
        {
            //LD.setNumber(80);
            speed = brake; //1
            direction = straight; //0
        }
        if(sensor_st.SENSOR_right_in <= CORNER_DIST && sensor_st.SENSOR_left_in <= CORNER_DIST)
        {
            // middle(25 - 256), left(0 - 25), right(0 - 25)
            LD.setNumber(40);
            sp = slow;      //2
            dir = straight; //0
        }
        else if(sensor_st.SENSOR_right_in <= CORNER_DIST)
        {
            //middle(> 25), left(> 25), right(0 - 25)
            LD.setNumber(30);
            sp = slow; //2
            dir = slight_left; //2
        }
        else if(sensor_st.SENSOR_left_in <= CORNER_DIST)
        {
            //middle(> 25), left(0 - 25), right(> 25)
            LD.setNumber(10);
            sp = slow; //2
            dir = slight_right; //4
        }
        else
        {
            //middle,left, right (> 25)
            LD.setNumber(90);
            sp = medium; //3
            dir = straight; //0
        }
    }
    if(!waitReverse)
    {
       if(prev_speed > reverse) //for brake, slow, medium, fast
        {
           transmit_to_motor(sp, dir);
           prev_speed = sp;
        }
        else //prev_speed == reverse
        {
            if(sensor_st.SENSOR_left_in >= sensor_st.SENSOR_right_in)
            {
                transmit_to_motor(slow, slight_left);
                prev_speed = slow;
            }
            else if(sensor_st.SENSOR_left_in < sensor_st.SENSOR_right_in)
            {
                transmit_to_motor(slow, slight_right);
                prev_speed = slow;
            }
        }
        isReverse = 0;
        LE.off(4);
    }
    else //reverse detected
    {
        if(isReverse == 1)
        {
            transmit_to_motor(brake, straight);
            prev_speed = brake; //added to compare with previous values
            LD.setNumber(1);
        }
        else
        {
            transmit_to_motor(reverse, straight);
            LE.on(4);
            prev_speed = reverse; //added to compare with previous values
            LD.setNumber(2);
        }
     }

    gChangeState = true;
    return true;
}
*/








