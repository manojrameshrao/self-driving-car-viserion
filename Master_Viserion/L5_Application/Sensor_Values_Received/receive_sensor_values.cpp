/*
 * receive_sensor_values.cpp
 *
 *  Created on: 28-Oct-2017
 *      Author: Admin
 */

#include "Sensor_Values_Received/receive_sensor_values.h"
#include "Transmit_Data_To_Motor/transmit_sensor_to_motor.h"
#include "_can_dbc/generated_Viserion.h"
#include "can.h"
#include "io.hpp"

const uint32_t                             SENSORS_VALUES__MIA_MS  = {10000};
const SENSORS_VALUES_t                     SENSORS_VALUES__MIA_MSG = {255,255,255};

SENSORS_VALUES_t sensor_st = {0};
SENSORS_VALUES_t minCheck = {5, 5, 5};
//SENSORS_VALUES_t maxCheck = {40,40,40};

#define CORNER_DIST 30 //15
#define MIDDLE_DIST 40 //20
#define MIN_DIST 10

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
                LE.on(1);
                if(sensor_st.SENSOR_left_in >= 5)
                {
                    minCheck.SENSOR_left_in = sensor_st.SENSOR_left_in;
                }
                else{
                    sensor_st.SENSOR_left_in = minCheck.SENSOR_left_in;
                }
                if(sensor_st.SENSOR_right_in >= 5)
                {
                    minCheck.SENSOR_right_in = sensor_st.SENSOR_right_in;
                }
                else{
                    sensor_st.SENSOR_right_in = minCheck.SENSOR_right_in;
                }
                if(sensor_st.SENSOR_middle_in >= 5)
                {
                    minCheck.SENSOR_middle_in = sensor_st.SENSOR_middle_in;
                }
                else{
                    sensor_st.SENSOR_middle_in = minCheck.SENSOR_middle_in;
                }
                checkSensorValues();
                break;
                /*  case 97 :
                LE.on(2);
                break;
            case 98 :
                LE.on(3);
                break;
            default :
                break;*/
        }
    }
    else
    {
        //do nothing
    }

    if(dbc_handle_mia_SENSORS_VALUES(&sensor_st, 100))
    {
        transmit_to_motor(1,0);
        LD.setNumber(19);
    }
    return true;
}
bool checkSensorValues()
{
    uint8_t speed = 0;
    uint8_t direction = 0;
    // bool reverse = false;
    if(sensor_st.SENSOR_middle_in <= MIDDLE_DIST) //middle detected
    {
        if(sensor_st.SENSOR_left_in <= CORNER_DIST && sensor_st.SENSOR_right_in <= CORNER_DIST ) //left detected
        {
            //all connected
            LD.setNumber(10);
            speed = 1; //brake
            direction = 0; //dont care
        }
        else if(sensor_st.SENSOR_right_in <= CORNER_DIST)
        {
            //right + middle
            /*if(sensor_st.SENSOR_right_in <= MIN_DIST || sensor_st.SENSOR_middle_in <= MIN_DIST)
            {
                LD.setNumber(22);
                speed = 1;//break
                direction = 0;//straight
            }
            else*/
            {
                LD.setNumber(20);
                LE.on(1);
                speed = 2; //maybe full left
                direction = 1; //dont care
            }

        }
        else if(sensor_st.SENSOR_left_in <= CORNER_DIST)
        {
            //left + middle
           /* if(sensor_st.SENSOR_right_in <= MIN_DIST || sensor_st.SENSOR_middle_in <= MIN_DIST)
            {
                LD.setNumber(32);
                speed = 1;
                direction = 0;
            }
            else*/
            {
                LD.setNumber(30);
                LE.on(2);
                speed = 2; //maybe full right
                direction = 3; //dont care
            }
        }
        else
        {
            //only middle
            if(sensor_st.SENSOR_middle_in <= CORNER_DIST)
            {
                LD.setNumber(40);
                LE.off(2);
                speed = 1; //break
                direction = 0; // straight
            }
            else
            {
                LD.setNumber(45);
                //LE.off()
                speed = 2; //slow
                direction = 0; //straight
            }
        }
    }
    else // no middle
    {
        if(sensor_st.SENSOR_right_in <= CORNER_DIST && sensor_st.SENSOR_left_in <= CORNER_DIST)
        {
            //middle + right
            LD.setNumber(50);
            LE.on(3);
            speed = 1;      //break : maybe add reverse
            direction = 0; // straight
        }
        else if(sensor_st.SENSOR_right_in <= CORNER_DIST)
        {
            //only right
            LD.setNumber(60);
            LE.off(3);
            speed = 2;
            direction = 2; //slight left
        }
        else if(sensor_st.SENSOR_left_in <= CORNER_DIST)
        {
            //only left
            LD.setNumber(70);
            LE.on(4);
            speed = 2;
            direction = 4; //slight right
        }
        else
        {
            LD.setNumber(80);
            LE.off(4);
            speed = 2;
            direction = 0;
            //maybe do nothing
        }
    }
    transmit_to_motor(speed, direction);
    return true;
}
