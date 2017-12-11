/*
 * receive_master_init.cpp
 *
 *  Created on: Oct 28, 2017
 *      Author: Miroslav Grubic
 */


#include <periodic_scheduler/send_and_receive/receive_master_init.h>
#include "can.h"
#include "_can_dbc/generated_Viserion.h"
#include "io.hpp"
#include "receive_master_init.h"


const uint32_t                             MASTER_INITIALIZE__MIA_MS = { 1000 };
const MASTER_INITIALIZE_t                  MASTER_INITIALIZE__MIA_MSG = { 0 };

MASTER_INITIALIZE_t master_init_msg = { 0 };


bool receive_master_init(bool &flag){
    can_msg_t can_msg;

    while (CAN_rx(can1, &can_msg, 0)){
        // Form the message header from the metadata of the arriving message
        dbc_msg_hdr_t can_msg_hdr;
        can_msg_hdr.dlc = can_msg.frame_fields.data_len;
        can_msg_hdr.mid = can_msg.msg_id;

        if(can_msg.msg_id == 81)
            flag= true;

        dbc_decode_MASTER_INITIALIZE(&master_init_msg, can_msg.data.bytes, &can_msg_hdr);
    }

    if(dbc_handle_mia_MASTER_INITIALIZE(&master_init_msg, 100)){
//        LD.setNumber(master_init_msg.MASTER_Init_Boards);
        LD.setNumber(0);
    }

    return flag;
}
