#ifndef HEADER_FILE
#define HEADER_FILE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define FALSE 0
#define TRUE 1

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */


char SET[5],UA[5];
SET[0]="0x7E";
SET[1]="0x03";
SET[2]="0x03";
SET[3]="0x00";
SET[4]="0x7E";

UA[0]="0x7E";
UA[1]="0x03";
UA[2]="0x07";
UA[3]="0x00";
UA[4]="0x7E";






#endif
