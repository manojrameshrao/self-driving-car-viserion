/*
 * bluetoothreceive.h
 *
 *  Created on: 27-Nov-2017
 *      Author: pratap
 */

#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_BLUETOOTHRECEIVE_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_BLUETOOTHRECEIVE_H_

#include <stdint.h>
#include <uart3.hpp>

void bluetoothreceiveinit(Uart3& u3);
void recieve_cmd_from_bluetooth(Uart3& u3);
void receiveAllCoordinates(Uart3& u3);

#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_BLUETOOTHRECEIVE_H_ */
