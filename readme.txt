=============================================================================
The package comes with two compression formats :
	- zip
	- 7zip

doxygen.zip :
To add doxygen (document generation) support, please also download
"doxygen.zip" and extract it to your sjsu_dev folder.

projects.zip :
This file is for some of the projects at package/projects directory.
This is when minor changes are made to projects, without updating the
entire zip or 7zip package.

----------------------------------------------------------------------------
  ********** PROBLEMS PROGRAMMING YOUR BOARD??? ***********
  ********** PROBLEMS RUNNING YOUR PROGRAM???   ***********
----------------------------------------------------------------------------
Use this one-time procedure :
- Open SJSU_Dev/ref_and_datasheets/flashmagic/flashmagic.exe
  - Select "LPC1758", Baud Rate: 38400, Your Com Port, and 4Mhz
- Flash this hex file: 
   - Pick a hex file from the projects directory
   - For example: "lpc1758_Hyperload_uart0_sdcard_spiflash.hex"
----------------------------------------------------------------------------


Revisions :
=============================================================================
11-08-2015 : 1) Added Python 3.5 to the package
             2) Updated to Eclipse Mars

01-24-2015 : 1) Just updated to latest project at this zip (with MPU)
             2) Updated EclipseARM plugin
             3) Updated to latest GCC 4.9-2014q4

08-23-2014 : 1) Updated GCC to "4.8 2014 Q2 update"
             2) "projects.zip" no longer included since original source is
                at Gitlab and can be "RESET" using "Team" Eclipse menu.

06-01-2014 : 1) Updated projects; added circular buffer class
             2) Included JRE path MINGW
             3) Updated MINGW to latest as of 03/22/2014
             4) Updated Windows FreeRTOS simulator to FreeRTOS 8.0
             5) Updated LPC1758 user manual PDF
             6) Added GIT repo for lpc projects

03-04-2014 : 1) Updated projects; fixed a nasty bug.

02-10-2014 : 1) Updated GCC to 4.8 with newlib 2.0

02-09-2014 : 1) Updated projects (See the readme of lpc1758_freertos)
             2) Fixed lpc1758_NoOS_HelloWorld project to use correct
                load offset with respect to new hyperload.

01-25-2014 : 1) Updated projects (See the readme of lpc1758_freertos)

01-05-2014 : 1) Updated GCC to 4.7.4 and removed Code Sourcery GCC
             2) Projects configured to use newlib-nano
                - Major reduction in code size and RAM usage.
             3) Modified Hyperload to send flash data from flash offset
             4) Modified hyperload bootloader to program from flash offset

12-13-2013 : 1) Fixed some things at the sample projects, mainly with I2C
             2) Added LPC electrical datasheet
             3) Updated hyperload bootloader again, with size change to 64K
                This way, we use all the 4K blocks, leaving 32K blocks for
                your project's code.  Due to the huge amount of padding, we
                should never need to change the bootloader again.

12-01-2013 : 1) Hyperload bootloader changed to 8K. Can program from SD card
             2) Projects' offset changed from 4K to 8K.  Older boards will
                need to be reflashed with new bootloader by using FlashMagic
             3) Cleaned up eclipse, and reinstalled latest GNUARM plugin
             4) Tested with Win 8 and Win 8.1.  Sometimes you may get an
		error while compiling, just recompile it as a workaround.

10-24-2013 : 1) Using pre-packaged java rather than rely us user's Java
             2) Changed toolchain from Yagarto to Code Sourcery
             3) Hyperload bootloader can now program from SPI flash
             4) Smaller program size by removing unused section by linker
             5) Changed "inline" to "static inline" for functions

10-23-2013 : Fixed lpc_pwm.cpp and improved RTC PLL search
10-20-2013 : RTC PLL can be used for LPC1758 project
10-10-2013 : Added RN-XV FreeRTOS task among other changes.
             See more info at projects/<project>/readme.txt
             Updated Hyperload.
             1) "SEND RESET" now opens COM port and resets the CPU
             2) If same file is programmed again, there will be a warning
09-29-2013 : Added experimental CAN driver; see more at projects/readme.txt
09-23-2013 : Turned off parallel builds again
09-21-2013 : Removed redundant "references" directory
             Used higher compression method to reduce download size
09-10-2013 : Updated Eclipse to Kepler and updated to latest GNU ARM Plugin
             Modified lpc1758 projects to use non-polling mesh; see readme.
09-09-2013 : Improvement to mesh algorithm; see lpc1758 project readme
             Disabled Parallel Builds because it would throw out an error
             sometimes.  You can renable by Project-->Properties, then browse
             to C/C++ Build --> Behavior.
08-28-2013 : More testing and improvement to mesh; see lpc1758 project readme
08-24-2013 : Updated lpc1758 projects.  See lpc1758_FreeRTOS/readme.txt
             The 7zip version mysteriously was an OLDER version. :(
08-21-2013 : Eclipse32_MINGW.bat was not compiling MINGW; fixed the path.
08-20-2013 : Updated lpc1758 projects.  See lpc1758_FreeRTOS/readme.txt
08-15-2013 : Enhanced mesh network API for lpc1758 projects
08-12-2013 : Cleaned up projects_mingw workspace
             Fixed lpc17xx projects (nordic directory had duplicate code).
08-09-2013 : First release note
             Updated projects.  See lpc1758_FreeRTOS/readme.txt
=============================================================================