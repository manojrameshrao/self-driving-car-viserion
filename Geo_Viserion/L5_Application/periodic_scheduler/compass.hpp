/*
 * compass.hpp
 *
 *  Created on: Nov 20, 2017
 *      Author: Aniket Dali
 */

#ifndef COMPASS_HPP_
#define COMPASS_HPP_

#include "stdint.h"

typedef enum
{
 UART0,
 UART2,
 UART3
}
UART_CHANNEL;


bool init_compass_serial(UART_CHANNEL UART_INTEFACE, unsigned int uart_baudrate);
bool get_compass_head( char * compass_head_pointer);
bool debug_compass_head(uint16_t compass_head_pointer);




#endif /* COMPASS_HPP_ */
