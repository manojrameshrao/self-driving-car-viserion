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
#include "adc0.h"
#include "stdio.h"
#include "gpio.hpp"
#include "utilities.h"
#include "send_sensors_data.h"


int adc1 = 0, adc2 = 0, adc3 = 0;
int distance1 = 0, distance2 = 0, distance3 = 0;
float voltage1 = 0, voltage2 = 0, voltage3 = 0;

const float adc_step = 3.3 / 4096;
const float voltage_scaling = 0.009765625;

GPIO leftSensorTrigger(P2_6);
GPIO rightSensorTrigger(P2_7);
GPIO middleSensorTrigger(P2_8);


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
bool period_init(void)
{
    CAN_init(can1, 100, 10, 10, NULL, NULL);
    CAN_reset_bus(can1);
    CAN_bypass_filter_accept_all_msgs();

    leftSensorTrigger.setAsOutput();
    leftSensorTrigger.setLow();

    rightSensorTrigger.setAsOutput();
    rightSensorTrigger.setLow();

    middleSensorTrigger.setAsOutput();
    middleSensorTrigger.setLow();

    LPC_PINCON->PINSEL1 |= (1 << 20); // ADC-3 is on P0.26, select this as ADC0.3
    LPC_PINCON->PINSEL3 |= (3 << 28); // ADC-4 is on P1.30, select this as ADC0.4
    LPC_PINCON->PINSEL3 |= (3 << 30); // ADC-5 is on P1.31, select this as ADC0.5

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
        CAN_reset_bus(can1);
}

void period_10Hz(uint32_t count)
{
    adc1 = adc0_get_reading(3);
    adc2 = adc0_get_reading(4);
    adc3 = adc0_get_reading(5);

    voltage1 = adc1 * adc_step;
    voltage2 = adc2 * adc_step;
    voltage3 = adc3 * adc_step;

    distance1 = voltage1 / voltage_scaling;
    distance2 = voltage2 / voltage_scaling;
    distance3 = voltage3 / voltage_scaling;

    printf("1: %d, 2: %d, 3: %d \n", distance1, distance2, distance3);

    if(count > 5){
        send_sensors_data(distance1, distance2, distance3);
    }
}

void period_100Hz(uint32_t count)
{
    if(count % 10 == 0){

        leftSensorTrigger.setHigh();
        delay_us(25);
        leftSensorTrigger.setLow();

        rightSensorTrigger.setHigh();
        delay_us(25);
        rightSensorTrigger.setLow();
    }

    if(count % 10 == 5){

        middleSensorTrigger.setHigh();
        delay_us(25);
        middleSensorTrigger.setLow();
    }
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
//    LE.toggle(4);
}
