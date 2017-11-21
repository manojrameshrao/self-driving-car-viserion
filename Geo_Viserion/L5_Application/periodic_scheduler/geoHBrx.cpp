/*
 * geoHeartbeat.cpp
 *
 *  Created on: 22-Oct-2017
 *      Author: Admin
 */

#include <stdio.h>
#include "can.h"
#include <periodic_scheduler/geoHBrx.h>
#include "_can_dbc/generated_Viserion.h"
#include "io.hpp"

const uint32_t                             HB_SENSORS__MIA_MS = 10000;
const HB_SENSORS_t                         HB_SENSORS__MIA_MSG = {.SENSOR_ALIVE = 0};
const uint32_t                             HB_MOTORS__MIA_MS = 10000;
const HB_MOTORS_t                          HB_MOTORS__MIA_MSG = {.MOTOR_ALIVE = 0};
const uint32_t                             HB_BT__MIA_MS = 10000;
const HB_BT_t                              HB_BT__MIA_MSG = {.BT_ALIVE = 0};
const uint32_t                             HB_MASTER__MIA_MS = 10000;
const HB_MASTER_t                          HB_MASTER__MIA_MSG = {.MASTER_ALIVE = 0};

HB_SENSORS_t sensor_msg = {0};
HB_MOTORS_t motors_msg = {0};
HB_BT_t bt_msg = {0};
HB_MASTER_t master_msg = {0};

void receive_heartbeats(void)
{
    //LE.setAll(0x00);
    can_msg_t can_msg;
    if(CAN_rx(can1, &can_msg, 0))
    {
        dbc_msg_hdr_t canMsg;
        canMsg.mid = can_msg.msg_id;
        canMsg.dlc = can_msg.frame_fields.data_len;

        switch(can_msg.msg_id)
        {
            //11 22 44 55
            case 95 :
                dbc_decode_HB_SENSORS(&sensor_msg,can_msg.data.bytes,&canMsg);
              //  printf("ge95");
                LD.setNumber(11);
                LE.on(1);
                break;
            case 96 :
                dbc_decode_HB_MOTORS(&motors_msg,can_msg.data.bytes,&canMsg);
                //printf("ge96");
                LD.setNumber(22);
                LE.on(2);
                break;
            case 98 :
                dbc_decode_HB_BT(&bt_msg,can_msg.data.bytes,&canMsg);
               // printf("ge98");
                LD.setNumber(44);
               LE.on(3);
                break;
            case 99 :
                dbc_decode_HB_MASTER(&master_msg,can_msg.data.bytes,&canMsg);
               // printf("ge99");
                LD.setNumber(55);
               LE.on(4);
                break;
        }

    }
    if(dbc_handle_mia_HB_SENSORS(&sensor_msg, 100))
    {
       LD.setNumber(10);
       LE.off(1);
    }
    if(dbc_handle_mia_HB_MOTORS(&motors_msg, 100))
    {
        LD.setNumber(20);
        LE.off(2);
    }
    if(dbc_handle_mia_HB_BT(&bt_msg, 100))
    {
        LD.setNumber(40);
        LE.off(3);
    }
    if(dbc_handle_mia_HB_MASTER(&master_msg, 100))
    {
        LD.setNumber(50);
        LE.off(4);
    }

}

