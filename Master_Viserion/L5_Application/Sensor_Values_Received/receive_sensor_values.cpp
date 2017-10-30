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
#include "io.hpp"

const uint32_t                             SENSORS_VALUES__MIA_MS  = {10000};
const SENSORS_VALUES_t                     SENSORS_VALUES__MIA_MSG = {255,255,255};

SENSORS_VALUES_t sensor_st = {0};

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
  //LE.off(1);
  uint8_t speed = 0;
  uint8_t direction = 0;
   if(sensor_st.SENSOR_left_in <= MIN_DIST)// || sensor_st.SENSOR_right_in<=MIN_DIST)
   {
       LD.setNumber(5);
       LE.on(2);
       speed = 2;
       direction = 4;

   }
   else if(sensor_st.SENSOR_right_in <= MIN_DIST)
   {
       LD.setNumber(6);
       LE.on(3);
       speed = 2;
       direction = 2;

   }
   else if(sensor_st.SENSOR_middle_in <= MIN_DIST)
   {
       LD.setNumber(6);
       LE.on(3);
       speed = 1;
       direction = 5;
   }
   else
   {
       LD.setNumber(7);
       LE.on(4);
       LE.off(2);
       LE.off(3);
       speed = 2;
       direction = 0;
   }

   transmit_to_motor(speed, direction);
/*
   if(sensor_st.SENSOR_right_in <= 10)
   {-
         LD.setNumber(8);
         LE.on(3);
         speed = 1;
         direction = 5;
         transmit_to_motor(speed, direction);
   }
   else
   {
         LD.setNumber(9);
         LE.off(3);
   }

   if(sensor_st.SENSOR_middle_in <= 10)
   {
       LD.setNumber(2);
       LE.on(4);
       speed = 1;
       direction = 5;
       transmit_to_motor(speed, direction);
   }
   else
   {*-
       LD.setNumber(1);
       LE.off(4);
   }
*/
   /*
    if(sensor_st.SENSOR_middle_in != 255) //middle present
    {

        //obstacle detected in front so check left and right

        if(sensor_st.SENSOR_right_in != 255)//right present
        {
            if(sensor_st.SENSOR_left_in != 255) //middle + right + left
            {
                //stop and take reverse and check with reverse sonar maybe
                //output = "stop : m + l + r and reverse";
              //   speed = 0;
                 direction = 5;
            }
            else //middle + right : // All values need to be tested
            {
                if((sensor_st.SENSOR_middle_in <= 10) || (sensor_st.SENSOR_right_in <= 10))
                {
                //    speed = 0;
                    direction = 5;
                    //output = "stop: m + r due to m or r and reverse + left";
                }
                else if((sensor_st.SENSOR_middle_in < 100) || (sensor_st.SENSOR_right_in < 100))
                {
                    //output = "take a sharp left : m or r";
                  //  speed = 2;
                    direction = 1;
                }
                else
                {
                    //output = "take normal left";
                    //go straight for some time
                    //speed = 1;
                    direction = 2;
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
                    direction = 5;
                }
                else if((sensor_st.SENSOR_middle_in < 100) && (sensor_st.SENSOR_left_in < 100))
                {
                    //output = "take sharp right : m";
                    direction = 3;
                }
                else
                {
                    //output = "take normal right";
                    //go straight for some time maybe
                    direction = 4;
                }
            }
            else // only middle present
            {
                if(sensor_st.SENSOR_middle_in <= 10)
                {
                    //output = "stop :m and reverse\n";
                    direction  = 5;
                }
                else if(sensor_st.SENSOR_middle_in < 100)
                {
                    //output = "Sharp left/left or right";
                    direction = 1;
                }
                else
                {
                    direction = 2;
                    //output = "Left or Right";
                    //go straight for some time
                }

            }

        }

    }
    else
    {
        //output = "go straight";
        direction = 0;
    }
    //transmit_to_motor(speed, direction);
    //LD.setLeftDigit(speed);
   */
    return true;//need to check
}
