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

bool take_decision(can_msg_t *crx,dbc_msg_hdr_t *rx)
{
    uint8_t speed = 0;
    uint8_t direction = 0;
    SEND_GEO_ANGLES_t geo_msg={0};
    if(crx->msg_id == 401)
    {
        dbc_decode_SEND_GEO_ANGLES(&geo_msg,crx->data.bytes,rx);
        int angle_diff =0,heading=0,bearing=0,dist=0;
        heading = geo_msg.SEND_HEAD;
        bearing = geo_msg.SEND_BEAR;
        dist = geo_msg.SEND_DIST_CHKPOINT;

/*
        if(dist == 0)
        {
            //stop
        }
        else
*/
        {
            if(heading>180)
            {
                heading-=360;
            }
            angle_diff = bearing - heading;

            if(angle_diff > 10 && angle_diff > 90) // use #define
            {
                speed = slow;
                direction = full_right;
            }
            else if(angle_diff > 10 && angle_diff < 90)
            {
                speed = slow;
                direction = slight_right;
            }
            else if(angle_diff < -10 && angle_diff < -90)
            {
                speed = slow;
                direction = full_left;
            }
            else if(angle_diff < -10 && angle_diff > -90)
            {
                speed = slow;
                direction = slight_left;
            }
            else
            {
                speed = medium;
                direction = straight;
            }
        }
        gChangeState = true;
    }


   return true;
}




