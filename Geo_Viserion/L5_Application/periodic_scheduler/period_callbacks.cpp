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
#include "printf_lib.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "can.h"
//#include "periodic_scheduler/geoHBtx.h"
//#include "periodic_scheduler/geoHBrx.h"
#include "GPS.h"
#include "compass.hpp"
#include "generated_Viserion.h"
#include "uart2.hpp"
#include <stdlib.h>
/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/* variable to receive CAN message */
can_msg_t can_received_message;
dbc_msg_hdr_t can_received_message_header;


/* communication with Master-module */
SEND_GEO_ANGLES_t angles_data;
dbc_msg_hdr_t encoded_geo_angles;

/* communication with BT-moudle */
SEND_NO_OF_CHECKPOINTS_t no_of_checkpoints;
COORDINATES_SENT_t cordinates_sent;
SEND_R_COORDINATES_t checkpoints;
SEND_COORDINATES_t current_coordinates;
DESTINATION_REACHED_t destination;
MASTER_START_CMD_t start;
bool start_flag = false;
/* GPS variables start
**/




/* GPS variables end
*/


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
    bool status = false;
    /* initalize compass */
    status = init_compass_serial(UART3,115200);
    if(status)
    {
       printf("compass serial interface initialized \n");
    }
    else
    {
       printf("compass serial interface not initialized \n");
    }


    /* call to initialize GPS module */
    init_GPS_module();
   // satelite[2]='\n';
    /* initialize CAN communication */
    CAN_init(can1, 100, 10, 10, NULL, NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);
    isNumberCheckPointsReceived = false;
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
    if(CAN_is_bus_off(can1))
    {
            CAN_reset_bus(can1);
    }
}

void period_10Hz(uint32_t count)
{
    bool status = false;
    float bearing_angle;
    /* variable to hold the current compass value */
    unsigned int  compass_head = 0;
    /* history_compass_head variable, holds the compass_head value */
    static unsigned int  history_compass_head = 0;
    static unsigned int  history_GPS_bear = 0;


    static unsigned int sendCordinate;
    static unsigned int checkpoint_index =0;


    /* CAN Reception */
    if(CAN_rx(can1,&can_received_message,0))
    {

        switch(can_received_message.msg_id)
        {
            /* receive number of checkpoints to the destination from BT module */
            case 504:
                dbc_decode_SEND_NO_OF_CHECKPOINTS (&no_of_checkpoints,can_received_message.data.bytes,&can_received_message_header);
                printf("number of check points received: %d",no_of_checkpoints.BT_NO_OF_COR);
             //   isNumberCheckPointsReceived = true;
                break;

            case 505:
                dbc_decode_COORDINATES_SENT(&cordinates_sent, can_received_message.data.bytes,&can_received_message_header);
                u0_dbg_printf("number of check points received :%d \n",checkpoint_index);
                isNumberCheckPointsReceived = true;
                break;

            case 500:
                //printf("500 received \n");
                 sendCordinate = true;
                break;

            case 503:
                 dbc_decode_SEND_R_COORDINATES(&checkpoints,can_received_message.data.bytes,&can_received_message_header);
                 uint32_t raw;
                     raw  = ((uint32_t)((can_received_message.data.bytes[0]))); ///< 8 bit(s) from B0
                     raw |= ((uint32_t)((can_received_message.data.bytes[1]))) << 8; ///< 8 bit(s) from B8
                     raw |= ((uint32_t)((can_received_message.data.bytes[2]))) << 16; ///< 8 bit(s) from B16
                     raw |= ((uint32_t)((can_received_message.data.bytes[3]) & 0x3f)) << 24; ///< 6 bit(s) from B24
                     checkpoints.SET_LATTITUDE = ((raw * 1e-06) + (-180));
                     raw  = ((uint32_t)((can_received_message.data.bytes[3] >> 6) & 0x03)); ///< 2 bit(s) from B30
                     raw |= ((uint32_t)((can_received_message.data.bytes[4]))) << 2; ///< 8 bit(s) from B32
                     raw |= ((uint32_t)((can_received_message.data.bytes[5]))) << 10; ///< 8 bit(s) from B40
                     raw |= ((uint32_t)((can_received_message.data.bytes[6]))) << 18; ///< 8 bit(s) from B48
                     raw |= ((uint32_t)((can_received_message.data.bytes[7]) & 0x0f)) << 26; ///< 4 bit(s) from B56
                     checkpoints.SET_LONGITUDE = ((raw * 1e-06) + (-180));


                 u0_dbg_printf("%lf",checkpoints.SET_LATTITUDE);
                 u0_dbg_printf("%lf",checkpoints.SET_LONGITUDE);
                 (checkpoints_array[checkpoint_index]).latitude = checkpoints.SET_LATTITUDE;
                 (checkpoints_array[checkpoint_index]).longitude = checkpoints.SET_LONGITUDE;
                 checkpoint_index++;
                 break;
            case 81:
                start_flag = true;
                break;
            default: break;
        }
    }






    //bearing_angle = get_bearing_angle();
    bearing_angle = get_bearing_angle_haversine();
   // printf("Haver:%f\n", get_bearing_angle_haversine());
    //printf("Normal:%f\n", bearing_angle);
    /* get the compass head (YAW) from Razor SEN-14001 */
    status = get_compass_head(&compass_head);
    if(status)
    {
        /* indication of Razor SEN-14001's communication with SJ1 */
        history_compass_head = compass_head;
    }
    else
    {
        /* Indication of RAZOR SEN14001 not able to send the data */
       compass_head = history_compass_head;
    }
//    if(compass_head > 180)
//        compass_head = compass_head - 360;
   // printf("Bearing: %f\n", bearing_angle);
  //  printf("Heading: %d\n", compass_head);
    angles_data.SEND_HEAD = compass_head;
    angles_data.SEND_BEAR = bearing_angle;
//    if(start_flag)
//    {
   //     printf("message sent \n");
        dbc_encode_and_send_SEND_GEO_ANGLES(&angles_data);
  //  }
    if(checkpoint_reached()&& GPS_ready())
    {
//        no_checkpoint_reached++;  //printf("destination reached");
//        if(checkpoint_index == no_checkpoint_reached)
//        {
            LE.toggle(4);
            destination.GEO_DESTINATION = checkpoint_index;
            dbc_encode_and_send_DESTINATION_REACHED(&destination);
 //       }
    }


    if(sendCordinate == true)
    {
        if(GPS_ready())
        {
            current_coordinates.SEND_LATTITUDE = current.latitude;
            current_coordinates.SEND_LONGITUDE = current.longitude;
            dbc_encode_and_send_SEND_COORDINATES(&current_coordinates);
            sendCordinate = false;
        }
    }
}

void period_100Hz(uint32_t count)
{

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{

}
