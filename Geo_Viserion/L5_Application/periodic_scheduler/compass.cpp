#include "stdio.h"
#include <string.h>
#include "compass.hpp"
#include "uart0.hpp"
#include "uart2.hpp"
#include "uart3.hpp"
#include "printf_lib.h"
#include "io.hpp"
#include <stdlib.h>
/* function: init_compass_serial
 * initializes serial interfaces to get values from Razor 9DOF IMU - SEN14001
 * UART_CHANNEL [in]: UART port number
 * uart_baudrate [in]: UART Baud rate
 * Returns :True(on success)/false(fail)
 */
Uart0 &u0 = Uart0::getInstance();
Uart2 &u2 = Uart2::getInstance();
Uart3 &u3 = Uart3::getInstance();

#ifdef OFFSET_CORRECTION
    static int16_t offset = 0;
    enum {
            sw1 = (1 << 0),
            sw2 = (1 << 1),
            sw3 = (1 << 2),
            sw4 = (1 << 3),
        };
#endif

bool init_compass_serial(UART_CHANNEL UART_INTERFACE,unsigned int uart_baudrate)
{
    bool rtn = false;
    switch(UART_INTERFACE)
    {
        case UART0:
                    rtn = u0.init(uart_baudrate,0,1);
                    break;
        case UART2:
                    rtn = u2.init(uart_baudrate,0,1);
                    break;
        case UART3:
                    rtn = u3.init(uart_baudrate,5,1);
                    break;
            default:
                    break;
    }
    return rtn;
}

/* function: get_compass_head
 * Returns where the Y axis of Razor 9DOF IMU - SEN14001's magnetometer points
 * variable to hold the compass_value[in]: unint16_t compass_header_pointer
 * Returns :True(on success)/false(fail)
 */
bool get_compass_head(unsigned int * compass_head_pointer)
{
    const char delimiter[2] = ",";
    bool rtn = false;
    char str[6]={0};
    static char * head_data = NULL;
    rtn = u3.gets(str,sizeof(str),10);

    if(rtn)
    {
        head_data = strtok(str, delimiter);
        if(NULL != head_data)
        {
            *compass_head_pointer = atoi(head_data);
#ifdef OFFSET_CORRECTION
            {
                *compass_head_pointer += offset;
            }
#endif
        }

    }
    return rtn;
}
/* function: init_compass_serial
 * Outputs the compass head of Razor 9DOF IMU - SEN14001 on SJ1 serial port
 * compass_header_angle(from the north)[in]: compass_head_pointer
 * Returns :True(on success)/false(fail)
 */
bool debug_compass_head(uint16_t compass_head_pointer)
{
    bool rtn = false;
    return rtn;
}

#ifdef OFFSET_CORRECTION
   void switch_calibration(void)
   {
       const uint8_t switches = SW.getSwitchValues();
       switch(switches)
       {
           case sw1 :
           case sw2 :
           case sw3 :
           case sw4 :
           default:
               break;
       }
   }
#endif
