#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_SENDONCAN_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_SENDONCAN_H_


void sendStartCoordinate();
void sendAllCordinates(double latti,double longi);
void sendEndOfCoordinares();
void sendCarStart();
void sendCarStop();
void sendsize(int ncheck);

#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_SENDONCAN_H_ */
