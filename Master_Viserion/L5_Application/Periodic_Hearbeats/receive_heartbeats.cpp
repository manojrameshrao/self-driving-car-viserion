/*
 * receive_heartbeats.cpp
 *
 *  Created on: Oct 23, 2017
 *      Author: Aakash
 */
#include <Periodic_Hearbeats/receive_heartbeats.h>
#include "_can_dbc/generated_Viserion.h"
#include "io.hpp"
#include <stdbool.h>
#include <stdio.h>

const uint32_t                             HB_SENSORS__MIA_MS  = {10000};
const HB_SENSORS_t                         HB_SENSORS__MIA_MSG = {0};
const uint32_t                             HB_MOTORS__MIA_MS  = {10000};
const HB_MOTORS_t                          HB_MOTORS__MIA_MSG = {0};
const uint32_t                             HB_GEO__MIA_MS  = {10000};
const HB_GEO_t                             HB_GEO__MIA_MSG = {0};
const uint32_t                             HB_BT__MIA_MS  = {10000};
const HB_BT_t                              HB_BT__MIA_MSG = {0};

HB_SENSORS_t sensor_hb_msg = {0};
HB_MOTORS_t motor_hb_msg = {0};
HB_GEO_t geo_hb_msg = {0};
HB_BT_t bt_hb_msg = {0};


bool receive_heartbeats(can_t canbus,uint32_t mia_inc_time)
{
   can_msg_t hb_rx_msg= {0};
   dbc_msg_hdr_t rx_hdr;

   while(CAN_rx(canbus,&hb_rx_msg,0))
   {
       rx_hdr.mid = hb_rx_msg.msg_id;
       rx_hdr.dlc = hb_rx_msg.frame_fields.data_len;

       switch(rx_hdr.mid)
       {
           case SENSORS_HB_en: //printf("SR\n");
                               dbc_decode_HB_SENSORS(&sensor_hb_msg,hb_rx_msg.data.bytes,&rx_hdr);
                               LE.set(1,1);
                               LD.setNumber(11);
                               break;
           case MOTOR_HB_en:   //printf("MR\n");
                               dbc_decode_HB_MOTORS(&motor_hb_msg,hb_rx_msg.data.bytes,&rx_hdr);
                               LE.set(2,1);
                               LD.setNumber(22);
                               break;
           case GEO_HB_en:     //printf("GR\n");
                               dbc_decode_HB_GEO(&geo_hb_msg,hb_rx_msg.data.bytes,&rx_hdr);
                               LE.set(3,1);
                               LD.setNumber(33);
                               break;
           case BT_HB_en:      //printf("BR\n");
                               dbc_decode_HB_BT(&bt_hb_msg,hb_rx_msg.data.bytes,&rx_hdr);
                               LE.set(4,1);
                               LD.setNumber(44);
                               break;
       }
   }

   if(dbc_handle_mia_HB_SENSORS(&sensor_hb_msg,mia_inc_time))
   {
       LE.set(1,0);
       LD.setNumber(10);
   }
   if(dbc_handle_mia_HB_MOTORS(&motor_hb_msg,mia_inc_time))
   {
       LE.set(2,0);
       LD.setNumber(20);
   }
   if(dbc_handle_mia_HB_GEO(&geo_hb_msg,mia_inc_time))
   {
       LD.setNumber(30);
       LE.set(3,0);
   }
   if(dbc_handle_mia_HB_BT(&bt_hb_msg,mia_inc_time))
   {
       LE.set(4,0);
       LD.setNumber(40);
   }

   return true; //maybe check for some error conditions?
}



