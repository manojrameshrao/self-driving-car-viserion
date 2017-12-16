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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "io.hpp"
#include "uart2.hpp"
#include "periodic_callback.h"
#include "generated_Viserion.h"
#include "GPS.h"
#include "compass.hpp"
//#define BEARING_TEST


/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 8);

/* variable to receive CAN message */
can_msg_t can_received_message;

/* communication with Master-module */
SEND_GEO_ANGLES_t angles_data;
dbc_msg_hdr_t encoded_geo_angles;

/* communication with BT-moudle */
DESTINATION_REACHED_t destination;
//MASTER_START_CMD_t start;

/* variable to hold the current compass value */
static unsigned int  compass_head = 0;

/* variable to hold start_message_status from master*/
static bool received_start_master = false;

/*variable to indicate car has arrived on one of the checkpoints*/
static bool checkpoint_viccinity = false;

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
      // printf("compass serial interface initialized \n");
    }
    else
    {
      // printf("compass serial interface not initialized \n");
        LE.set(3,1);
    }


    /* call to initialize GPS module */
    init_GPS_module();
   // satelite[2]='\n';
    /* initialize CAN communication */
    CAN_init(can1, 100, 20, 20, NULL, NULL);
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
            LE.set(3,1);
            CAN_reset_bus(can1);

    }
    else if(received_start_master == true && ((count%3)==0))
    {
            LE.off(3);
            send_current_car_cordinates_run(true);
    }

#ifdef BEARING_TEST
   double bearing_angle;
   bearing_angle = get_bearing_angle_haversine();
   u0_dbg_printf("Bearing: %f\n", bearing_angle);
#endif
}

void period_10Hz(uint32_t count)
{
    bool status = false;
//    float bearing_angle = 0;
    /* history_compass_head variable, holds the compass_head value */
    static unsigned int  history_compass_head = 0;
    /*  get the compass head (YAW) from Razor SEN-14001*/
    status = get_compass_head(&compass_head);
    if(status)
    {
        /*  indication of Razor SEN-14001's communication with SJ1*/
        history_compass_head = compass_head;
    }
    else
    {
       /*  Indication of RAZOR SEN14001 not able to send the data*/
       compass_head = history_compass_head;
    }
 //   u0_dbg_printf("%d \n",compass_head);
}

void period_100Hz(uint32_t count)
{

    bool start = false;
    /*  CAN Reception */
    if(CAN_rx(can1,&can_received_message,0))
    {
        start = GPS_receive_data_processing(can_received_message);
        if(start)
        {
            received_start_master = true;
        }

        calculate_and_send_angles(received_start_master);
    }
#ifdef BEARING_TEST
    //calculate_and_send_angles(true);
#endif
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{

}

/* function: calculate_and_send_angles
 * Calculate and send angles based on start flag
 */
void calculate_and_send_angles(bool start)
{
   float bearing_angle = 0;
#ifdef BEARING_TEST
   bearing_angle = get_bearing_angle_haversine();
   u0_dbg_printf("Bearing: %f\n", bearing_angle);
#endif
   if(start)
   {

       bearing_angle = get_bearing_angle_haversine();

     //  bearing_angle = get_bearing_angle();
#ifdef DEBUG
        printf("Bearing: %f\n", bearing_angle);
        printf("Heading: %d\n", compass_head);
#endif

       angles_data.SEND_HEAD = compass_head;
       angles_data.SEND_BEAR = bearing_angle;

      // LE.toggle(3);
       /*  Send 401 msg to master to send the calculated angles*/
       dbc_encode_and_send_SEND_GEO_ANGLES(&angles_data);

       /*  Check if the intermediate checkpoint has reached*/
       if(checkpoint_reached())
       {
           LE.toggle(2);

       }
       else
       {
           LE.off(2);
       }
       if(destination_reached())
       {
           /*  Send 405 to indicate destination reached*/
           LE.toggle(4);
           destination.GEO_DESTINATION = 1;
           dbc_encode_and_send_DESTINATION_REACHED(&destination);
       }
       else
       {
           LE.off(4);
           destination.GEO_DESTINATION = 0;
       }
    }
   else
   {
       /*Get GPS data if start is not available else get_bearing angle is calculating the angle*/
         get_GPS_data();
   }

}
