/*
 * receive_sensor_values.cpp
 *
 *  Created on: 28-Oct-2017
 *      Author: Admin
 */

#include "Sensor_Values_Received/receive_sensor_values.h"
#include "Transmit_Data_To_Motor/transmit_sensor_to_motor.h"
#include <Periodic_Hearbeats/receive_heartbeats.h>
#include "_can_dbc/generated_Viserion.h"
#include "can.h"
#include "io.hpp"

const uint32_t                             SENSORS_VALUES__MIA_MS  = {2000};
const SENSORS_VALUES_t                     SENSORS_VALUES__MIA_MSG = {255,255,255};

SENSORS_VALUES_t sensor_st = {0};

#define CORNER_DIST 30  //25 - Previous Commit Values
#define MIDDLE_DIST 35 //25 - Previous Commit Values
#define MIN_MIDDLE_DIST 10
#define MIN_CORNER_DIST 15

bool receiveSensorValues(void)
{
    can_msg_t can_msg = {0};
    dbc_msg_hdr_t msgRx = {0};
    if(CAN_rx(can1, &can_msg, 0))
    {
        msgRx.dlc = can_msg.frame_fields.data_len;
        msgRx.mid = can_msg.msg_id;
        switch(msgRx.mid)
        {
            case sensor_Data_Id :
                dbc_decode_SENSORS_VALUES(&sensor_st, can_msg.data.bytes, &msgRx);
                LE.set(2, 1);
                checkSensorValues();
                break;
        }
    }
    if(dbc_handle_mia_SENSORS_VALUES(&sensor_st, 10))
    {
        //transmit_to_motor(3,0); //should be brake
        LD.setNumber(19);
        LE.set(2, 0);
    }
    return true;
}

bool checkSensorValues()
{
    uint8_t speed = 0;
    uint8_t direction = 0;
    if(sensor_st.SENSOR_middle_in <= MIN_MIDDLE_DIST) //if middle : 0 to 10
    {
        LD.setNumber(5);
        speed = brake; //1
        direction = straight; //0
    }
    else if(sensor_st.SENSOR_middle_in > MIN_MIDDLE_DIST && sensor_st.SENSOR_middle_in <= MIDDLE_DIST) //middle detected 11 - 25
    {
        if(sensor_st.SENSOR_left_in <= CORNER_DIST && sensor_st.SENSOR_right_in <= CORNER_DIST )
        {
            //left(0-25) + middle(11 - 25) + right(0-25)
            LD.setNumber(60);
            speed = brake; // 1
            direction = straight; //0
        }
        else if(sensor_st.SENSOR_right_in <= CORNER_DIST) // right(0 - 25) + middle(11 - 25)
        {
            LD.setNumber(70);
            speed = slow; //2
            direction = full_left; //1
        }
        else if(sensor_st.SENSOR_left_in <= CORNER_DIST)  //left(0 - 25) + middle (11 -25)
        {
            LD.setNumber(50);
            speed = slow; //2
            direction = full_right; //3
        }
        else //middle(11 - 25)
        {
            LD.setNumber(20);
            speed = slow; //slow, 2
            direction = slight_left; //2
        }
    }
    else // (middle > 25)
    {
       /*if(sensor_st.SENSOR_left_in <= MIN_CORNER_DIST && sensor_st.SENSOR_right_in <= MIN_CORNER_DIST)
        {
            //LD.setNumber(80);
            speed = brake; //1
            direction = straight; //0
        }*/
        if(sensor_st.SENSOR_right_in <= CORNER_DIST && sensor_st.SENSOR_left_in <= CORNER_DIST)
        {
            // middle(25 - 256), left(0 - 25), right(0 - 25)
            LD.setNumber(40);
            speed = slow;      //2
            direction = straight; //0
        }
        else if(sensor_st.SENSOR_right_in <= CORNER_DIST)
        {
            //middle(> 25), left(> 25), right(0 - 25)
            LD.setNumber(30);
            speed = slow; //2
            direction = slight_left; //2
        }
        else if(sensor_st.SENSOR_left_in <= CORNER_DIST)
        {
            //middle(> 25), left(0 - 25), right(> 25)
            LD.setNumber(10);
            speed = slow; //2
            direction = slight_right; //4
        }
        else
        {
            //middle,left, right (> 25)
            LD.setNumber(90);
            speed = medium; //3
            direction = straight; //0
        }
    }
    transmit_to_motor(speed, direction);
    return true;
}
