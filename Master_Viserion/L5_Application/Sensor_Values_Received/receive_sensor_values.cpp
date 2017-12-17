/*
 * receive_sensor_values.cpp
 *
 *  Created on: 28-Oct-2017
 *      Author: Admin
 */
#include <CommonHeader/commonVariables.h>
#include "receive_sensor_values.h"
#include "Sensor_Values_Received/receive_sensor_values.h"
#include "Transmit_Data_To_Motor/transmit_sensor_to_motor.h"
#include <Periodic_Hearbeats/receive_heartbeats.h>
#include "io.hpp"

const uint32_t                             SENSORS_VALUES__MIA_MS  = {2000};
const SENSORS_VALUES_t                     SENSORS_VALUES__MIA_MSG = {255,255,255};

SENSORS_VALUES_t sensor_st = {0};

#define CORNER_DIST 30  //25 - Previous Commit Values
#define MIDDLE_DIST 35 //25 - Previous Commit Values
#define REAR_DIST   20 // Not sure

#define MIN_MIDDLE_DIST 10
#define MIN_CORNER_DIST 15
#define MIN_REAR_DIST   15 //10 too small : does not stop : slow brake

#define REVERSE_COUNT 2

#define USE_SENSORS
//#define USE_GEO

bool receiveSensorValues(unsigned int speed,unsigned int direction,can_msg_t *crx,dbc_msg_hdr_t *rx)
{
    //can_msg_t can_msg = {0};
    dbc_msg_hdr_t msgRx = {0};
    //if(CAN_rx(can1, &can_msg, 0))
    {
        msgRx.dlc = crx->frame_fields.data_len;
        msgRx.mid = crx->msg_id;

        rx->dlc = crx->frame_fields.data_len;
        rx->mid = crx->msg_id;
        switch(rx->mid)
        {
            case Sensor_Data_Id :
                if(dbc_decode_SENSORS_VALUES(&sensor_st, crx->data.bytes, &msgRx))
                {
                    LE.set(2, 1);
                    checkSensorValues(speed,direction);
                }
                break;
        }
    }

    if(dbc_handle_mia_SENSORS_VALUES(&sensor_st, 10))
    {
        transmit_to_motor(brake,straight); //should be brake
        LD.setNumber(19);
        LE.set(2, 0);
    }
    return true;
}

bool checkSensorValues(unsigned int speed,unsigned int direction) // these values will be gGeoSpeed, gGeoDirection
{
    static uint8_t isReverse = 0;
    static uint8_t prev_speed = brake; //first iteration
    static bool waitReverse = false;

#ifdef USE_SENSORS
    if(!waitReverse)
    {
        if(sensor_st.SENSOR_middle_in <= MIN_MIDDLE_DIST) //if middle : 0 to 10
        {
            LD.setNumber(5);
            speed = brake; //1
            direction = straight; //0
            if(sensor_st.SENSOR_back_in <= MIN_REAR_DIST)
            {
                waitReverse = true;
            }
        }
        else if(sensor_st.SENSOR_middle_in > MIN_MIDDLE_DIST && sensor_st.SENSOR_middle_in <= MIDDLE_DIST) //middle detected 11 - 25
        {
            if(sensor_st.SENSOR_left_in <= CORNER_DIST && sensor_st.SENSOR_right_in <= CORNER_DIST )
            {
                //left(0-25) + middle(11 - 25) + right(0-25)
                LD.setNumber(60);
                speed = brake; // 1
                direction = straight; //0
                waitReverse = true;
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
#ifdef USE_GEO
            else // No obstacle Detected
            {
                //middle,left, right (> 25)
                LD.setNumber(90);
                //speed = medium; //3
                //direction = straight; //0

            }
#else
            else        //middle,left, right (> 25)
            {
                LD.setNumber(90);
                speed = medium; //3
                direction = straight; //0
            }
#endif
        } //end of sensor big loop
    }
    else  //once reverse is detected
    {
        if(isReverse < REVERSE_COUNT && sensor_st.SENSOR_back_in > MIN_REAR_DIST) //1,2 : send reverse
        {
                speed = reverse;
                direction = straight;
                isReverse++;
        }
        else if(sensor_st.SENSOR_back_in <= MIN_REAR_DIST)
        {
            speed = brake;
            direction = straight;
            isReverse = 0;
            waitReverse = false;
        }
        else
        {
            isReverse = 0;
            waitReverse = false;
        }
    }
    if(!waitReverse)
    {
       if(prev_speed > reverse) //for brake, slow, medium, fast
        {
               transmit_to_motor(speed, direction);
               prev_speed = speed;
        }
        else //prev_speed == reverse
        {
            if(sensor_st.SENSOR_left_in >= sensor_st.SENSOR_right_in)
            {
                transmit_to_motor(slow, slight_left);
                prev_speed = slow;
            }
            else if(sensor_st.SENSOR_left_in < sensor_st.SENSOR_right_in)
            {
                transmit_to_motor(slow, slight_right);
                prev_speed = slow;
            }
        }
        isReverse = 0;
        LE.off(4);
    }
    else //waitReverse detected
    {
        transmit_to_motor(speed, direction);
        prev_speed = reverse;
        LE.on(4);
        //cannot make isReverse = 0 here
    }


#else
    transmit_to_motor(speed, direction);
#endif
    gChangeState = true;
    return true;
}


/*
else //reverse detected
{
    if(isReverse == 1)
    {
        transmit_to_motor(brake, straight);
        prev_speed = brake; //added to compare with previous values
       // LD.setNumber(1);
    }
    else
    {
        if(isReverse < 3)
        {
            if(sensor_st.SENSOR_back_in <= MIN_REAR_DIST)
            {
                transmit_to_motor(brake, straight);
                rearDetected = true;
            }
            else
            {
                transmit_to_motor(reverse, straight);
            }
        }
        else
        {
            isReverse = 0;
            transmit_to_motor(brake, straight);

        }
        LE.on(4);
        prev_speed = reverse; //added to compare with previous values
       // LD.setNumber(2);
    }
 }
*/
