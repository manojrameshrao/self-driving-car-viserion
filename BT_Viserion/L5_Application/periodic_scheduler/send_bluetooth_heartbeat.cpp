/*
 * send_bluetooth_heartbeat.cpp
 *
 *  Created on: 24-Oct-2017
 *      Author: pratap desai
 */
#include "send_bluetooth_heartbeat.h"
#include "generated_Viserion.h"

void send_bluetooth_hartbeat(void){
    HB_BT_t bluetooth_hb = { 0 };
    bluetooth_hb.BT_ALIVE = 1;

    dbc_encode_and_send_HB_BT(&bluetooth_hb);
}
