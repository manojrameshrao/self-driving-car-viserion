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

#include "gpio.hpp"
#include "utilities.h"  // delay_us()
#include "eint.h"
#include "stdio.h"



GPIO leftSensorTrigger(P2_6);
GPIO rightSensorTrigger(P2_7);
GPIO middleSensorTrigger(P2_7);



int START, STOP, LEFT_DISTANCE, RIGHT_DISTANCE, MIDDLE_DISTANCE = 0;


enum sensor_t {
    left,
    right,
    middle,
    wait
};
sensor_t sensor = left;


void leftSensorStartISR(void){
    START = sys_get_uptime_us();
    LE.on(1);
}

void leftSensorStopISR(void){
    STOP = sys_get_uptime_us();
    LEFT_DISTANCE = (STOP - START) / 147;
    sensor = right;
    LE.off(1);
}

void rightSensorStartISR(void){
    START = sys_get_uptime_us();
    LE.on(2);
}

void rightSensorStopISR(void){
    STOP = sys_get_uptime_us();
    RIGHT_DISTANCE = (STOP - START) / 147;
    sensor = middle;
    LE.off(2);
}

void middleSensorStartISR(void){
    START = sys_get_uptime_us();
    LE.on(3);
}

void middleSensorStopISR(void){
    STOP = sys_get_uptime_us();
    MIDDLE_DISTANCE = (STOP - START) / 147;
    sensor = left;
    LE.off(3);
}

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
    leftSensorTrigger.setAsOutput();
    leftSensorTrigger.setLow();

    rightSensorTrigger.setAsOutput();
    rightSensorTrigger.setLow();

    middleSensorTrigger.setAsOutput();
    middleSensorTrigger.setLow();

    uint8_t Pin_2_0 = 0;
    eint3_enable_port2(Pin_2_0, eint_rising_edge, leftSensorStartISR);
    eint3_enable_port2(Pin_2_0, eint_falling_edge, leftSensorStopISR);

    uint8_t Pin_2_1 = 1;
    eint3_enable_port2(Pin_2_1, eint_rising_edge, rightSensorStartISR);
    eint3_enable_port2(Pin_2_1, eint_falling_edge, rightSensorStopISR);

    uint8_t Pin_2_2 = 2;
    eint3_enable_port2(Pin_2_2, eint_rising_edge, middleSensorStartISR);
    eint3_enable_port2(Pin_2_2, eint_falling_edge, middleSensorStopISR);

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
//    LOG_INFO("left: %d, right: %d\n", LEFT_DISTANCE, RIGHT_DISTANCE);
//    printf("left: %d, right: %d\n", LEFT_DISTANCE, RIGHT_DISTANCE);
}

void period_10Hz(uint32_t count)
{
    printf("________________________________________\n");
//    LE.toggle(2);
}

void period_100Hz(uint32_t count)
{
    switch(sensor){

        case left:
            leftSensorTrigger.setHigh();
            delay_us(20);
            break;

        case right:
            rightSensorTrigger.setHigh();
            delay_us(20);
            break;

        case middle:
            middleSensorTrigger.setHigh();
            delay_us(20);
            break;

        case wait:
            break;
    }
    printf("left: %d, right: %d, middle: %d\n", LEFT_DISTANCE, RIGHT_DISTANCE, MIDDLE_DISTANCE);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
    LE.toggle(4);
}
