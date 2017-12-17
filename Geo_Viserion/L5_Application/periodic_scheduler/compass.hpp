/*
 * compass.hpp
 *
 *  Created on: Nov 20, 2017
 *      Author: Aniket Dali
 */

#ifndef COMPASS_HPP_
#define COMPASS_HPP_

#include "stdint.h"
#include "utilities.h"


#define OFFSET_CORRECTION
typedef enum
{
 UART0,
 UART2,
 UART3
}
UART_CHANNEL;

enum {
    sw1 = (1 << 0),
    sw2 = (1 << 1),
    sw3 = (1 << 2),
    sw4 = (1 << 3),
};
bool init_compass_serial(UART_CHANNEL UART_INTEFACE, unsigned int uart_baudrate);
bool get_compass_head(unsigned int * compass_head);
bool debug_compass_head(uint16_t compass_head_pointer);

#ifdef OFFSET_CORRECTION
   void switch_calibration(void);
#endif

#endif /* COMPASS_HPP_ */
