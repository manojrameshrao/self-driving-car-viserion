/*
 * geo_find_directions.cpp
 *
 *  Created on: 10-Dec-2017
 *      Author: Admin
 */
#include "geo_find_directions.h"
#include <CommonHeader/commonVariables.h>
#include <Periodic_Hearbeats/receive_heartbeats.h>
#include "io.hpp"

bool take_decision(can_msg_t *crx,dbc_msg_hdr_t *rx, unsigned int& pSpeed, unsigned int& pDirection)
{
    if(crx->msg_id == Geo_Angles)
    {
        SEND_GEO_ANGLES_t geo_msg={0};
        rx->dlc=crx->frame_fields.data_len;
        rx->mid=crx->msg_id;
        dbc_decode_SEND_GEO_ANGLES(&geo_msg,crx->data.bytes,rx);
        int angle_diff =0,heading=0,bearing=0,dist=0;
        heading = geo_msg.SEND_HEAD;
        bearing = geo_msg.SEND_BEAR;
        dist = geo_msg.SEND_DIST_CHKPOINT;

        if(dist == 0)
        {
            //glow leds and check for distance range
        }
        if(heading>ANGLE_OFFSET)
        {
            heading-=ANGLE_COMPENSATION;
        }
        angle_diff = bearing - heading;

        if(angle_diff > ANGLE_RANGE && angle_diff > HARD_TURN_ANGLE) // use #define
        {
            pSpeed = slow;
            pDirection = full_right;
        }
        else if(angle_diff > ANGLE_RANGE && angle_diff < HARD_TURN_ANGLE)
        {
            pSpeed = slow;
            pDirection = slight_right;
        }
        else if(angle_diff < -ANGLE_RANGE && angle_diff < -HARD_TURN_ANGLE)
        {
            pSpeed = slow;
            pDirection = full_left;
        }
        else if(angle_diff < -ANGLE_RANGE && angle_diff > -HARD_TURN_ANGLE)
        {
            pSpeed = slow;
            pDirection = slight_left;
        }
        else
        {
            pSpeed = medium;
            pDirection = straight;
        }
        gChangeState = true;
    }
   return true;
}




