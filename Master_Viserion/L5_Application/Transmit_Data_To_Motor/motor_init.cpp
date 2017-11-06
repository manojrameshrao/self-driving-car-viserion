/*
 * motor_init.cpp
 *
 *  Created on: 30-Oct-2017
 *      Author: Admin
 */
#include "Transmit_Data_To_Motor/motor_init.h"
#include "can.h"
#include "generated_Viserion.h"

bool motor_init(void)
{
    MASTER_INITIALIZE_t masterInit = {0};
    masterInit.MASTER_Init_Boards = (1 << startMotor);
    can_msg_t motorInit = {0};
    dbc_msg_hdr_t msg_init = dbc_encode_MASTER_INITIALIZE(motorInit.data.bytes, &masterInit);
    motorInit.frame_fields.data_len = msg_init.dlc;
    motorInit.msg_id = msg_init.mid;

    return CAN_tx(can1, &motorInit, 0);
}



