
#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_DBC_APP_SEND_CAN_MSG_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_DBC_APP_SEND_CAN_MSG_H_

#include <stdint.h>

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]);


#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_DBC_APP_SEND_CAN_MSG_H_ */
