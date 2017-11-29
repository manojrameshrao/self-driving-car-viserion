
#include "send_bluetooth_heartbeat.h"
#include "generated_Viserion.h"
#include "io.hpp"

void send_bluetooth_hertbeat(void){
    HB_BT_t bluetooth_hb = { 0 };
    bluetooth_hb.BT_ALIVE = 1;

    dbc_encode_and_send_HB_BT(&bluetooth_hb);
 }
