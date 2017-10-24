/*
 * send_heartbeat.cpp
 *
 *  Created on: Oct 23, 2017
 *      Author: Aakash
 */
#include <Periodic_Hearbeats/send_heartbeats.h>
#include "generated_Viserion.h"

bool send_heartbeat(can_t canbus)
{
    HB_MASTER_t hbmsg  = {0};
    hbmsg.MASTER_ALIVE =  1;

    can_msg_t hb_can={0};
    dbc_msg_hdr_t msg_hdr = dbc_encode_HB_MASTER(hb_can.data.bytes,&hbmsg);
    hb_can.msg_id = msg_hdr.mid ;
    hb_can.frame_fields.data_len = msg_hdr.dlc;

    return CAN_tx(canbus,&hb_can,0);
}



