
#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_RECEIVEFROMCAN_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_RECEIVEFROMCAN_H_

#include "uart3.hpp"

void receiveallcanmsges(Uart3& u3);
void sendDetails(Uart3& u3,bool stats);

#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_RECEIVEFROMCAN_H_ */
