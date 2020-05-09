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

