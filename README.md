# version
 
KEIL uvision IDE doesn't include a build number update system. 
Therefore, I have written a small program to edit a templete file.

the templete file includes,

#define MAJOR 1

#define MINOR 0

#define RELEASE 0

#define BUILD 3245

#define LASTBUILD "Sat May 09 14:28:47 2020"


the program only manipulates the BUILD and LAST BUILD Value.

Prior to the compilation process keil calls the program and BUILD number is incremented by 1.
LAST BUILD value is updated.

How to use:

C:\buildincr.exe [templete path]

C:\buildincr.exe C:\....\version.h

after the call program prints out the updated version and the time value, respectively

v1.0.0.3246
"Sat May 09 15:28:47 2020"

and also templete file is updated.


