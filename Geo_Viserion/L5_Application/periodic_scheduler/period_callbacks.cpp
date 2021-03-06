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
/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);
/* compass variables */
SEND_COMPASS_HEAD_t compass_pointer;
dbc_msg_hdr_t encoded_compass_pointer;
/* GPS variables start
**/



char buffer[200]={0};
char satelite[3];

uint8_t no_sat_locked;
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
      status = init_compass_serial(UART3,115200);
      if(status)
      {
          printf("compass serial interface initialized \n");
      }
      else
      {
          printf("compass serial interface not initialized \n");
      }
    CAN_init(can1, 100, 10, 10, NULL, NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);

    init_GPS_module(); //call to initialize GPS module
    satelite[2]='\n';
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
    //LE.toggle(1);
    if(CAN_is_bus_off(can1))
    {
      //printf("Can bus is off\n");
            CAN_reset_bus(can1);
    }

   // geo_heartbeat();
}

void period_10Hz(uint32_t count)
{


    GPS_data.gets(buffer,sizeof(buffer),0);

    //u0_dbg_printf("\nb-%s",buffer);

    satelite[0]=buffer[46];
    satelite[1]=buffer[47];
    no_sat_locked = atoi(satelite);
   // u0_dbg_printf("%d\n",no_sat_locked);
    if(no_sat_locked>=3)
    {
        LE.on(1);
    }
    else
    {
        LE.off(1);
    }
    LD.setNumber(no_sat_locked);

    bool status = false;
    unsigned int  compass_head = 0;
    unsigned int  history_compass_head =0;

  // receive_heartbeats();
    status = get_compass_head(&compass_head);
    if(status)
    {
        LE.toggle(2);
        history_compass_head = compass_head;
        u0_dbg_printf("compass_head %d \n",compass_head);
        compass_pointer.SEND_HEAD = compass_head;
        dbc_encode_and_send_SEND_COMPASS_HEAD(&compass_pointer);
    }
    else
    {
        LE.toggle(3);
     //  printf("failed to get the compass head \n");
      // compass_pointer.SEND_HEAD = history_compass_head;
      // dbc_encode_and_send_SEND_COMPASS_HEAD(&compass_pointer);
    }
}

void period_100Hz(uint32_t count)
{
    //LE.toggle(3);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
  //  LE.toggle(4);
}
