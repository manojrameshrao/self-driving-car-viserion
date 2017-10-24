/*
 * dbc_app_send_can_msg.cpp
 *
 *  Created on: 24-Oct-2017
 *      Author: pratap Desai
 */


#include "dbc_app_send_can_msg.h"
#include "can.h"
#include "string.h"


bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}
