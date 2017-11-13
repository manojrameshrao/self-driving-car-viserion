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
const SENSORS_VALUES_t                     SENSORS_VALUES__MIA_MSG = {5,5,5};

SENSORS_VALUES_t sensor_st = {0};

#define CORNER_DIST 25 //15 25
#define MIDDLE_DIST 25//20  25
#define MIN_MIDDLE_DIST 10
#define MIN_CORNER_DIST 10

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
                checkSensorValues();
                break;
        }
    }
    if(dbc_handle_mia_SENSORS_VALUES(&sensor_st, 100))
    {
        transmit_to_motor(3,0); // now : slow + straight, prev : brake
        LD.setNumber(19);
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
        speed = 1; //break
        direction = 0; // straight
    }
    else if(sensor_st.SENSOR_middle_in > MIN_MIDDLE_DIST && sensor_st.SENSOR_middle_in <= MIDDLE_DIST) //middle detected 11 - 25
    {
        if(sensor_st.SENSOR_left_in <= CORNER_DIST && sensor_st.SENSOR_right_in <= CORNER_DIST )
        {
            //left(0-25) + middle(11 - 25) + right(0-25)
            LD.setNumber(50);
            speed = 1; //prev : brake, now : reverse
            direction = 0; //straight
        }
        else if(sensor_st.SENSOR_right_in <= CORNER_DIST) // right(0 - 25) + middle(11 - 25)
        {
            LD.setNumber(70);
            speed = 2; //slow
            direction = 1; //full left
        }
        else if(sensor_st.SENSOR_left_in <= CORNER_DIST)  //left(0 - 25) + middle (11 -25)
        {
            LD.setNumber(60);
            speed = 2; //slow
            direction = 3; //full right
        }
        else //middle(11 - 25)
        {
            LD.setNumber(20);
            speed = 2; //slow
            direction = 2; // now : slight left prev :straight
        }
    }
    else // (middle > 25)
    {
       /*if(sensor_st.SENSOR_left_in <= MIN_CORNER_DIST && sensor_st.SENSOR_right_in <= MIN_CORNER_DIST)
        {
            //LD.setNumber()
            speed = 1; //brake
            direction = 0; // straight
        }*/
        if(sensor_st.SENSOR_right_in <= CORNER_DIST && sensor_st.SENSOR_left_in <= CORNER_DIST)
        {
            // middle(25 - 256), left(0 - 25), right(0 - 25)
            LD.setNumber(40);
            speed = 2;      //go straight and slow,  break : maybe add reverse
            direction = 0; // straight
        }
        else if(sensor_st.SENSOR_right_in <= CORNER_DIST)
        {
            //middle(> 25), left(> 25), right(0 - 25)
            LD.setNumber(30);
            speed = 2; //slow
            direction = 2; //slight left
        }
        else if(sensor_st.SENSOR_left_in <= CORNER_DIST)
        {
            //middle(> 25), left(0 - 25), right(> 25)
            LD.setNumber(10);
            speed = 2; //slow
            direction = 4; //slight right
        }
        else
        {
            //middle,left, right (> 25)
            LD.setNumber(90);
            speed = 3; //slow
            direction = 0; //straight
        }
    }
    transmit_to_motor(speed, direction);
    return true;
}
