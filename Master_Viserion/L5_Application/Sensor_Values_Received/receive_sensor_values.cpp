/*
 * receive_sensor_values.cpp
 *
 *  Created on: 28-Oct-2017
 *      Author: Admin
 */

#include "Sensor_Values_Received/receive_sensor_values.h"
#include "_can_dbc/generated_Viserion.h"
#include "can.h"

bool receiveSensorValues(void)
{
    can_msg_t can_msg;
    if(CAN_rx(can1, &can_msg, 0))
    {

    }
    else
    {
        //do nothing
    }
}

