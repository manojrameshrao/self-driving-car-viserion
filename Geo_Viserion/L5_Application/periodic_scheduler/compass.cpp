#include "stdio.h"
#include "compass.hpp"
#include "uart0.hpp"
#include "uart2.hpp"
#include "uart3.hpp"
#include "printf_lib.h"
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
// = {0};
bool init_compass_serial(UART_CHANNEL UART_INTERFACE,unsigned int uart_baudrate)
{
    bool rtn = false;
    switch(UART_INTERFACE)
    {
        case UART0:

                   // p_uart = &u0;
                    rtn = u0.init(uart_baudrate,0,1);
                    break;
        case UART2:
                   //p_uart = &u2;
                    rtn = u2.init(uart_baudrate,0,1);
                    break;
        case UART3:
                    //p_uart = &u3;
                    rtn = u3.init(uart_baudrate,5,1);
                    break;
            default:
                    break;
    }
   // printf("initialized %d \n",rtn);
    return rtn;
}

/* function: get_compass_head
 * Returns where the Y axis of Razor 9DOF IMU - SEN14001's magnetometer points
 * variable to hold the compass_value[in]: unint16_t compass_header_pointer
 * Returns :True(on success)/false(fail)
 */
bool get_compass_head(unsigned int * compass_head_pointer)
{
    bool rtn = false;
    char str[6]={0};
    char remaining;
    rtn = u3.gets(str,sizeof(str),10);
    //u3.flush();

   // u0_dbg_printf("str %s \n",str);
    if(rtn)
    {
     //    u0_dbg_printf("return success \n");
        *compass_head_pointer = atoi(str);
    }
    u3.getChar(&remaining,0);
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
