/*
 * receive_sensor_values.cpp
 *
 *  Created on: 28-Oct-2017
 *      Author: Admin
 */

#include "Sensor_Values_Received/receive_sensor_values.h"
#include "Sensor_Values_Received/transmit_sensor_to_motor.h"
#include "_can_dbc/generated_Viserion.h"
#include "can.h"

const uint32_t                             SENSORS_VALUES__MIA_MS  = {10000};
const SENSORS_VALUES_t                     SENSORS_VALUES__MIA_MSG = {0,0,0};

SENSORS_VALUES_t sensor_st;

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
                checkSensorValues();
                break;
        }

    }
    else
    {
        //do nothing

    }

    if(dbc_handle_mia_SENSORS_VALUES(&sensor_st, 100))
    {
        //do something
    }
    return true;
}

bool checkSensorValues()
{
   uint8_t speed = 0;
    uint8_t direction = 0;

    if(sensor_st.SENSOR_middle_in != 255) //middle present
    {
        //obstacle detected in front so check left and right
        if(sensor_st.SENSOR_right_in != 255)//right present
        {
            if(sensor_st.SENSOR_left_in != 255) //middle + right + left
            {
                //stop and take reverse and check with reverse sonar maybe
                //output = "stop : m + l + r and reverse";
            }
            else //middle + right : // All values need to be tested
            {
                if((sensor_st.SENSOR_middle_in <= 10) || (sensor_st.SENSOR_right_in <= 10))
                {
                    //output = "stop: m + r due to m or r and reverse + left";
                }
                else if((sensor_st.SENSOR_middle_in < 100) || (sensor_st.SENSOR_right_in < 100))
                {
                    //output = "take a sharp left : m or r";
                }
                else
                {
                    //output = "take normal left";
                    //go straight for some time
                }
            }
        }
        else //middle + left
        {
            //cout << "l :" << l << "\n";
            if(sensor_st.SENSOR_left_in != 255) //check if left present : check all the values
            {
                if((sensor_st.SENSOR_middle_in <= 10) && (sensor_st.SENSOR_left_in <= 10))
                {
                    //output = "stop : m + l due to m or l and reverse + right";
                }
                else if((sensor_st.SENSOR_middle_in < 100) && (sensor_st.SENSOR_left_in < 100))
                {
                    //output = "take sharp right : m";
                }
                else
                {
                    //output = "take normal right";
                    //go straight for some time maybe
                }
            }
            else // only middle present
            {
                if(sensor_st.SENSOR_middle_in <= 10)
                {
                    //output = "stop :m and reverse\n";
                }
                else if(sensor_st.SENSOR_middle_in < 100)
                {
                    //output = "Sharp left/left or right";
                }
                else
                {
                    //output = "Left or Right";
                    //go straight for some time
                }

            }

        }
    }
    else
    {
        //output = "go straight";
    }
    transmit_direction(direction);
    transmit_speed(speed);

    return true;//need to check
}
