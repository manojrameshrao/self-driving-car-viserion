/*****************************************************************************
$Work file     : display.cpp $
Description    : This file contains the Initialization of display and sensor tasks.
Project(s)     : Smart Health Gear
Compiler       : Cross ARM GCC
OS			   : RTOS
Original Author: $ Aniket Dali
$Author        : $ Aniket Dali
$Date          : $ 26 May 2017
$Revision      : 1.0 $
*****************************************************************************/
/****************************************************************************/
/*                       INCLUDE FILES                                      */
/****************************************************************************/
#include "display.hpp"
#include "printf_lib.h"
#include "utilities.h"
#include "stdint.h"
#include "LPC17xx.h"
#include "stdlib.h"
#include "rtc.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "eint.h"
#include "semphr.h"
#include "stdio.h"
#include "timers.h"
#include "lpc_timers.h"
#include <sstream>
#include "uart3.hpp"
extern "C"
{
	#include "gpio.h"
}
using namespace std;

 int DIRECTION_INIT =0;
 int SPEED_INIT =0;
 int SENSOR_LEFT_INIT =0;
 int BATTERY_INIT =0;

 void setDIRECTION_INIT(int setdir){
	 DIRECTION_INIT=setdir;
 }
 void setSPEED_INIT(int setspeed){
	 SPEED_INIT=setspeed;
  }
 void setSENSOR_LEFT_INIT(int setgps){
	 SENSOR_LEFT_INIT=setgps;
  }
 void setBATTERY_INIT(int setbattery){
	 BATTERY_INIT=setbattery;
  }


/****************************************************************************/
/*                        VARIABLES AND MACROS                              */
/****************************************************************************/
// global buffer to convert integers into ascii to be sent over SPI to Display
char buff[50] = {ZERO};

SemaphoreHandle_t SPEED_REFRESH	               = NULL;
SemaphoreHandle_t DIRECTION_REFRESH	      		   = NULL;
SemaphoreHandle_t SENSOR_LEFT_REFRESH				   = NULL;
SemaphoreHandle_t BATTERY_REFRESH     			   = NULL;
// Routine to configure Power to drive LCD
void LCDPower();


/**************** Character Array Operations ****************************/
// Routine to Trim the character array
char * trim(char * str,uint8_t start, uint8_t end);
// Routine to convert Integer to ASCII
char* itoa(int num, char* str, int base);
// Routine to reverse the character string
void reverse(char str[], int length);

/****************************************************************************/
/*                       FUNCTION DEFINITIONS                               */
/****************************************************************************/



/*----------------------------------------------------------------------------
Function    :  display_Task (Constructor)
Inputs      :  None
Processing  :  This function is Constructor for the display_Task. It configures
			   the SSP interface and port pins to interact with LCD module ILI9341
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
display_Task :: display_Task(uint8_t priority) :scheduler_task("display", 10240, priority)
{

	SSP0_power(ON);
	// clear the settings
	LPC_SC->PCLKSEL1 &= ~(0b11<<10);
	// set PCLK = CLK
	LPC_SC->PCLKSEL1 |= (0b01<<10);
	// SCK1
	GPIOSetMode(0,15,FUNC3);
	// MISO
	GPIOSetMode(0,17,FUNC3);
	// MOSI
	GPIOSetMode(0,18,FUNC3);
	// SCK speed = CPU / 2 = 24 Mhz
    LPC_SSP0->CPSR = 2;
	// set the data transfer to 8 bits.
	LPC_SSP0->CR0  =   0b0111;
	// Turn on SSP module
	SSP0_enable();
	// CS
	GPIOSetDir(0,29,OUTPUT);
	// RESET
	GPIOSetDir(0,30,OUTPUT);
	// DC/RS
	GPIOSetDir(1,19,OUTPUT);
	// LED pin
	GPIOSetDir(1,28,OUTPUT);
	// Turn on the power to LED driver circuit
	GPIOSetValue(1,28,1);
}
/*----------------------------------------------------------------------------
Function    :  display_Task (run)
Inputs      :  None
Processing  :  This function is run function for the display_Task. It de-queues
			   data from sensor queues, maintains the display state and refreshes
			   the display to display current time and sensor values.
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
bool display_Task::run(void* p)
{

	// Display initial  sensor values ( All 0's)
	xSemaphoreGive(SPEED_REFRESH);
	xSemaphoreGive(DIRECTION_REFRESH);
	xSemaphoreGive(SENSOR_LEFT_REFRESH);
	xSemaphoreGive(BATTERY_REFRESH);

	while(1)
	{

		displayScrn1();
//vTaskDelay(500);
xSemaphoreGive(SPEED_REFRESH);
	xSemaphoreGive(DIRECTION_REFRESH);
	xSemaphoreGive(SENSOR_LEFT_REFRESH);
	xSemaphoreGive(BATTERY_REFRESH);

	}
	return 1;
}
/*----------------------------------------------------------------------------
Function    :  LCDPower()
Inputs      :  None
Processing  :  This function controls the power to LCD
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void LCDPower()
{
	static bool Power = SET;
	if(Power)
	{
		// Turn Off the LCD
		GPIOSetValue(1,28,0);
		Power = 0;
	}
	else
	{
		// Turn ON the LCD
		GPIOSetValue(1,28,1);
		Power = 1;
	}
}
/*----------------------------------------------------------------------------
Function    :  display_Task(init)
Inputs      :  None
Processing  :  This function Initializes LCD ILI9341, Creates Queues, Semaphores
			   and mutex for signaling the sensor tasks, acquiring the data.
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
bool display_Task::init(void)
{

	// Give a nudge on Reset pin of LCD to initiate the setup process
	 display_RST_dessert();
	 display_RST_assert();
	 delay_ms(5);
	 display_RST_dessert();
	 delay_ms(20);
	 display_RST_assert();
	 delay_ms(150);

	/*********************** commands to configure ILI9340I **********************/
	  writecommand(0xEF);
	  writedata(0x03);
	  writedata(0x80);
	  writedata(0x02);

	  writecommand(0xCF);
	  writedata(0x00);
	  writedata(0XC1);
	  writedata(0X30);

	  writecommand(0xED);
	  writedata(0x64);
	  writedata(0x03);
	  writedata(0X12);
	  writedata(0X81);

	  writecommand(0xE8);
	  writedata(0x85);
	  writedata(0x00);
	  writedata(0x78);

	  writecommand(0xCB);
	  writedata(0x39);
	  writedata(0x2C);
	  writedata(0x00);
	  writedata(0x34);
	  writedata(0x02);

	  writecommand(0xF7);
	  writedata(0x20);

	  writecommand(0xEA);
	  writedata(0x00);
	  writedata(0x00);

	  // Power control
	  writecommand(ILI9340_PWCTR1);
	  // VRH[5:0]
	  writedata(0x23);

	  // Power control
	  writecommand(ILI9340_PWCTR2);
	  // SAP[2:0];BT[3:0]
	  writedata(0x10);

	  // VCM control
	  writecommand(ILI9340_VMCTR1);
	  writedata(0x3e);
	  writedata(0x28);

	  // VCM control2
	  writecommand(ILI9340_VMCTR2);
	  writedata(0x86);

	  // Memory Access Control
	  writecommand(ILI9340_MADCTL);
	  writedata(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);

	  writecommand(ILI9340_PIXFMT);
	  writedata(0x55);

	  writecommand(ILI9340_FRMCTR1);
	  writedata(0x00);
	  writedata(0x18);

	  // Display Function Control
	  writecommand(ILI9340_DFUNCTR);
	  writedata(0x08);
	  writedata(0x82);
	  writedata(0x27);

	  // 3Gamma Function Disable
	  writecommand(0xF2);
	  writedata(0x00);

	  //Gamma curve selected
	  writecommand(ILI9340_GAMMASET);
	  writedata(0x01);

	  // Set Gamma
	  writecommand(ILI9340_GMCTRP1);
	  writedata(0x0F);
	  writedata(0x31);
	  writedata(0x2B);
	  writedata(0x0C);
	  writedata(0x0E);
	  writedata(0x08);
	  writedata(0x4E);
	  writedata(0xF1);
	  writedata(0x37);
	  writedata(0x07);
	  writedata(0x10);
	  writedata(0x03);
	  writedata(0x0E);
	  writedata(0x09);
	  writedata(0x00);

	  // Set Gamma
	  writecommand(ILI9340_GMCTRN1);
	  writedata(0x00);
	  writedata(0x0E);
	  writedata(0x14);
	  writedata(0x03);
	  writedata(0x11);
	  writedata(0x07);
	  writedata(0x31);
	  writedata(0xC1);
	  writedata(0x48);
	  writedata(0x08);
	  writedata(0x0F);
	  writedata(0x0C);
	  writedata(0x31);
	  writedata(0x36);
	  writedata(0x0F);

	  // Exit Sleep mode
	  writecommand(ILI9340_SLPOUT);
	  delay_ms(120);
	  // Turn the display on
	  writecommand(ILI9340_DISPON);

	  // Make the background black
	  clearScrn();
	 // Create a binary semaphores to signal sensor parameters display refresh
	 SPEED_REFRESH        = xSemaphoreCreateBinary();
	 DIRECTION_REFRESH        = xSemaphoreCreateBinary();
	 SENSOR_LEFT_REFRESH        = xSemaphoreCreateBinary();
	 BATTERY_REFRESH        = xSemaphoreCreateBinary();

     return 1;
}

/*----------------------------------------------------------------------------
Function    :  clearscrn1()
Inputs      :  None
Processing  :  This function clears sensor screen
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::clearScrn1(void)
{
	drawFastVLine(5,0,200,ILI9340_BLACK);
	drawFastVLine(142,0,200,ILI9340_BLACK);
	drawFastVLine(235,0,200,ILI9340_BLACK);

	drawFastHLine(5,0,231,ILI9340_BLACK);
	drawFastHLine(5,50,231,ILI9340_BLACK);
	drawFastHLine(5,100,231,ILI9340_BLACK);
	drawFastHLine(5,150,231,ILI9340_BLACK);
	drawFastHLine(5,200,231,ILI9340_BLACK);
	drawString("Heart Rate",12,20,2,ILI9340_BLACK);
	drawString("Blood Oxygen",12,70,2,ILI9340_BLACK);
	drawString("Body Temp",12,120,2,ILI9340_BLACK);
	drawString("step count",12,170,2,ILI9340_BLACK);
	// Clear the readings
	fillRect(148,20,235,190,ILI9340_BLACK);

}
void display_Task::clearDIRECTION_INIT(void)
{
	fillRect(145,55,86,35,ILI9340_BLACK);
}
/*----------------------------------------------------------------------------
Function    :  clearBS()
Inputs      :  None
Processing  :  This function clears Bits per second parameters
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::clearSPEED_INIT(void)
{
	fillRect(145,15,86,35,ILI9340_BLACK);
}
/*----------------------------------------------------------------------------
Function    :  clearBT()
Inputs      :  None
Processing  :  This function clears Body Temperature parameters
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::clearSENSOR_LEFT_INIT(void)
{
	fillRect(145,105,86,35,ILI9340_BLACK);
}
/*----------------------------------------------------------------------------
Function    :  clearST()
Inputs      :  None
Processing  :  This function clears Step parameters
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::clearBATTERY_INIT(void)
{
	fillRect(145,165,86,35,ILI9340_BLACK);
}
/*----------------------------------------------------------------------------
Function    :  clearScrn()
Inputs      :  None
Processing  :  This function clears entire the screen
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::clearScrn(void)
{
	fillRect(0,0,240,320,ILI9340_BLACK);
}
/*----------------------------------------------------------------------------
Function    :  clearScrn3()
Inputs      :  None
Processing  :  This function clears Step parameters
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
Function    :  displayScrn3()
Inputs      :  None
Processing  :  This function displays sensor screen
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::displayScrn1(void)
{


	drawFastVLine(5,0,200,ILI9340_WHITE);
	drawFastVLine(142,0,200,ILI9340_WHITE);
	drawFastVLine(235,0,200,ILI9340_WHITE);

	drawFastHLine(5,0,231,ILI9340_WHITE);
	drawFastHLine(5,50,231,ILI9340_WHITE);
	drawFastHLine(5,100,231,ILI9340_WHITE);
	drawFastHLine(5,150,231,ILI9340_WHITE);
	drawFastHLine(5,200,231,ILI9340_WHITE);
	drawString("Speed",12,20,2,ILI9340_YELLOW);
	drawString("Direction",12,70,2,ILI9340_YELLOW);
	drawString("Sensor_Left",12,120,2,ILI9340_YELLOW);
	drawString("Battery",12,170,2,ILI9340_YELLOW);

	// Is there any change in BPS?
	if(xSemaphoreTake(SPEED_REFRESH,50))
	{
		itoa(SPEED_INIT,buff,10);
		clearSPEED_INIT();
		drawString(buff,148,20,2,ILI9340_YELLOW);
	}

	// Is there any change in Oxygen?
	if(xSemaphoreTake(DIRECTION_REFRESH,50))
	{
		itoa(DIRECTION_INIT,buff,10);
		clearDIRECTION_INIT();
		drawString(buff,148,70,2,ILI9340_YELLOW);
	}

	// Is there any change in Body Temp?
	if(xSemaphoreTake(SENSOR_LEFT_REFRESH,50))
	{
		itoa(SENSOR_LEFT_INIT,buff,10);
		clearSENSOR_LEFT_INIT();
		drawString(buff,148,120,2,ILI9340_YELLOW);
	}
	// Is there any change in Steps?
	if(xSemaphoreTake(BATTERY_REFRESH,50))
	{
		itoa(BATTERY_INIT,buff,10);
		clearBATTERY_INIT();
		drawString(buff,148,170,2,ILI9340_YELLOW);
	}

}

/*----------------------------------------------------------------------------
Function    :  UART3_init()
Inputs      :  None
Processing  :  This function Powers on SSP0 interface
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::SSP0_power(uint8_t mode)
{
	if(mode)
	{
		LPC_SC->PCONP |= (mode<<21);
	}
	else
	{
		LPC_SC->PCONP &= ~(mode<<21);
	}
}
/*----------------------------------------------------------------------------
Function    :  SSP0_byte_transfer()
Inputs      :  uint8_t send_byte
Processing  :  This function Powers on SSP0 interface
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
uint8_t display_Task::SSP0_byte_transfer (uint8_t send_byte)
{

	while (SSP0_TXfull());
    LPC_SSP0->DR = send_byte;

    while(SSP0_busy());

    return LPC_SSP0->DR;

}
/*----------------------------------------------------------------------------
Function    :  SSP0_enable()
Inputs      :  None
Processing  :  This function Enables SSP0 interface
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::SSP0_enable  (void)
{
	LPC_SSP0->CR1 |=   (0b1   << 1);
}
/*----------------------------------------------------------------------------
Function    :  SSP0_disable()
Inputs      :  None
Processing  :  This function Disables SSP0 interface
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::SSP0_disable (void)
{
	LPC_SSP0->CR1 &=  ~(0b1   << 1);
}
/*----------------------------------------------------------------------------
Function    :  SSP0_TXfull()
Inputs      :  None
Processing  :  This function checks whether the transmit queue is full?
Outputs     :  None
Returns     :  True, if busy, False if not full
Notes       :  None
----------------------------------------------------------------------------*/
bool display_Task::SSP0_TXfull(void)
{
	uint32_t mask = (0b1<<1);
	bool return_val = !(LPC_SSP0->SR & mask);
	return (return_val);
}
/*----------------------------------------------------------------------------
Function    :  SSP0_busy()
Inputs      :  None
Processing  :  This function checks whether the SSP interface is busy
Outputs     :  None
Returns     :  True, if busy, False if not full
Notes       :  None
----------------------------------------------------------------------------*/
bool display_Task::SSP0_busy(void)
{
	return (((LPC_SSP0->SR ) & (0b01 << 4)));
}
/*----------------------------------------------------------------------------
Function    :  display_CS_assert()
Inputs      :  None
Processing  :  This function asserts CS
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::display_CS_assert(void)
{
	GPIOSetValue(0,29,HIGH);
}
/*----------------------------------------------------------------------------
Function    :  display_CS_dessert()
Inputs      :  None
Processing  :  This function desserts CS
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::display_CS_dessert(void)
{
	GPIOSetValue(0,29,LOW);
}
/*----------------------------------------------------------------------------
Function    :  display_RST_assert()
Inputs      :  None
Processing  :  This function asserts RST
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::display_RST_assert(void)
{
	GPIOSetValue(0,30,HIGH);
}
/*----------------------------------------------------------------------------
Function    :  display_RST_dessert()
Inputs      :  None
Processing  :  This function desserts RST
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::display_RST_dessert(void)
{
	GPIOSetValue(0,30,LOW);
}
/*----------------------------------------------------------------------------
Function    :  display_DC_assert()
Inputs      :  None
Processing  :  This function makes D/C High
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::display_DC_assert(void)
{
	GPIOSetValue(1,19,HIGH);
}
/*----------------------------------------------------------------------------
Function    :  display_DC_dessert()
Inputs      :  None
Processing  :  This function makes D/C low
Outputs     :  None
Returns     :  None
Notes       :  None
----------------------------------------------------------------------------*/
void display_Task::display_DC_dessert(void)
{
	GPIOSetValue(1,19,LOW);
}

/*----------------------------------------------------------------------------
Function    :  reverse()
Inputs      :  char str[], int length
Processing  :  A utility function to reverse a string
Outputs     :  None
Returns     :  None
Notes       :  code sample taken from http://www.geeksforgeeks.org
----------------------------------------------------------------------------*/
 void reverse(char str[], int length)
 {
     int start = 0;
     int end = length -1;
     while (start < end)
     {
         swap(*(str+start), *(str+end));
         start++;
         end--;
     }
 }
 /*----------------------------------------------------------------------------
 Function    :  itoa()
 Inputs      :  int num, char* str, int base
 Processing  :  A utility function to covert integer into ascii
 Outputs     :  None
 Returns     :  char*
 Notes       :  code sample taken from http://www.geeksforgeeks.org
 ----------------------------------------------------------------------------*/
 char* itoa(int num, char* str, int base)
 {
     int i = 0;
     bool isNegative = false;

     /* Handle 0 explicitely, otherwise empty string is printed for 0 */
     if (num == 0)
     {
         str[i++] = '0';
         str[i] = '\0';
         return str;
     }

     // In standard itoa(), negative numbers are handled only with
     // base 10. Otherwise numbers are considered unsigned.
     if (num < 0 && base == 10)
     {
         isNegative = true;
         num = -num;
     }

     // Process individual digits
     while (num != 0)
     {
         int rem = num % base;
         str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
         num = num/base;
     }

     // If number is negative, append '-'
     if (isNegative)
         str[i++] = '-';

     // Append string terminator
     str[i] = '\0';

     // Reverse the string
     reverse(str, i);

     return str;
 }

 /****************************************************************************/
 /*                      ADA FRUIT GFX Library for ILI9430                   */
 /****************************************************************************/

void display_Task::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{

  writecommand(ILI9340_CASET); // Column addr set
  writedata(x0 >> 8);
  writedata(x0 & 0xFF);     // XSTART
  writedata(x1 >> 8);
  writedata(x1 & 0xFF);     // XEND

  writecommand(ILI9340_PASET); // Row addr set
  writedata(y0>>8);
  writedata(y0);     // YSTART
  writedata(y1>>8);
  writedata(y1);     // YEND

  writecommand(ILI9340_RAMWR); // write to RAM
}

void display_Task::drawPixel(int16_t x, int16_t y, uint16_t color)
{

	  if((x < 0) ||(x >= 240) || (y < 0) || (y >= 320)) return;

	  setAddrWindow(x,y,x+1,y+1);

	  //digitalWrite(_dc, HIGH);
	  display_DC_assert();
	  //digitalWrite(_cs, LOW);
	  display_CS_dessert();

	  SSP0_byte_transfer(color >> 8);
	  SSP0_byte_transfer(color);

	  //digitalWrite(_cs, HIGH);
	  display_CS_assert();
	//  printf("pixel executed \n");
}

int display_Task::drawChar(int16_t x, int16_t y, unsigned char c,
		  uint16_t color, uint16_t bg, uint8_t size)
{

//		    if(!gfxFont) { // 'Classic' built-in font

		        if((x >= _width)            || // Clip right
		           (y >= _height)           || // Clip bottom
		           ((x + 6 * size - 1) < 0) || // Clip left
		           ((y + 8 * size - 1) < 0))   // Clip top
		            return 1;

		        if((c >= 176)) c++; // Handle 'classic' charset behavior

		    //    startWrite();
		        for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
		            uint8_t line = pgm_read_byte(&font[c * 5 + i]);
		            for(int8_t j=0; j<8; j++, line >>= 1) {
		                if(line & 1) {
		                    if(size == 1)
		                    	drawPixel(x+i, y+j, color);
		                    else
		                        writeFillRect(x+i*size, y+j*size, size, size, color);
		                } else if(bg != color) {
		                    if(size == 1)
		                    	drawPixel(x+i, y+j, bg);
		                    else
		                        writeFillRect(x+i*size, y+j*size, size, size, bg);
		                }
		            }
		        }
		        if(bg != color) { // If opaque, draw vertical line for last column
		            if(size == 1) writeFastVLine(x+5, y, 8, bg);
		            else          writeFillRect(x+5*size, y, size, 8*size, bg);
		        }

		        return((5.0+0.4)*size);
		  //      endWrite();
}

void display_Task::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{

    // Overwrite in subclasses if desired!
    fillRect(x,y,w,h,color);

}


void display_Task::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    for (int16_t i=x; i<x+w; i++) {
        writeFastVLine(i, y, h, color);
    }

}


void display_Task::writeFastVLine(int16_t x, int16_t y,
        int16_t h, uint16_t color)
{
	drawFastVLine(x, y, h, color);

}

void display_Task::drawFastVLine(int16_t x, int16_t y,
        int16_t h, uint16_t color)
{
	writeLine(x, y, x, y+h-1, color);
}

void display_Task::drawFastHLine(int16_t x, int16_t y,
        int16_t h, uint16_t color)
{
	writeLine(x, y, x+h-1, y, color);
}

void display_Task::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
        	drawPixel(y0, x0, color);
        } else {
        	drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}


void display_Task::drawString(const char *string, int poX, int poY, int size,uint16_t color)
{
    int sumX = 0;

    while(*string)
    {
        int xPlus = drawChar(poX, poY, *string,color, ILI9340_BLACK,  size);
        sumX += xPlus;
        *string++;
        poX += xPlus;
    }

}
void  display_Task::writecommand(uint8_t command_byte)
{
	// DC- low
	display_DC_dessert();
	//CS- low
	display_CS_dessert();
	// SSP exchange
    SSP0_byte_transfer(command_byte);
	//CS- HIGH
	display_CS_assert();

}
void  display_Task::writedata(uint8_t data_byte)
{

	// DC- HIGH
	display_DC_assert();
	//CS- low
	display_CS_dessert();
	// SSP exchange
	SSP0_byte_transfer(data_byte);
	//CS- HIGH
	display_CS_assert();

}

char * trim(char * str,uint8_t start, uint8_t end)
{

		int j = 0;
		str = str+start-1;

		for(int k=0;k<20;k++){buff[k]=0;}
		while(start!=end)
		{
			buff[j] = *str;
			str++;
			j++;
			start++;

		}
		buff[j]='\0';

		return &buff[0];

}

void display_Task::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color)
{
    // Update in subclasses if desired!
    if(x0 == x1){
        if(y0 > y1) _swap_int16_t(y0, y1);
        drawFastVLine(x0, y0, y1 - y0 + 1, color);
    } else if(y0 == y1){
        if(x0 > x1) _swap_int16_t(x0, x1);
        drawFastHLine(x0, y0, x1 - x0 + 1, color);
    } else {

        writeLine(x0, y0, x1, y1, color);

    }
}
void display_Task::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

/*===================================================================
// $Log: $1.0 AVD:Added comments to increase the readability
//
//--------------------------------------------------------------------*/

