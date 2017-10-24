netload.exe readme :

What does it do?
Netload can transfer a file from your system to SJ-One board.

How does it work?
You need to have the RN-XV (wifi) module installed.
You need to have the RN-XV FreeRTOS task running.
You need to have the terminal task get a command from RN-XV Uart
in addition to the UART0.

What can I use it for?
You can transfer a new binary (program) file to the SJ-One Board.
Once the file is there, you can issue: "flash my_proj.bin" and
the board will reboot with the new file.