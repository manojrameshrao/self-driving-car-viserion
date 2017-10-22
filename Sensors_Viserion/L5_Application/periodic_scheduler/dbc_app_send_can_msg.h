/*
 * dbc_app_send_can_msg.h
 *
 *  Created on: Oct 21, 2017
 *      Author: ja
 */

#ifndef DBC_APP_SEND_CAN_MSG_H_
#define DBC_APP_SEND_CAN_MSG_H_

#include <stdint.h>

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]);


#endif /* DBC_APP_SEND_CAN_MSG_H_ */
