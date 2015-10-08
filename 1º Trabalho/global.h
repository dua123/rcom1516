#ifndef HEADER_FILE
#define HEADER_FILE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include<errno.h>

#define FALSE 0
#define TRUE 1

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */


unsigned char SET[5];


unsigned char UA[5];
/*UA[0]=0x7E;
UA[1]=0x03;
UA[2]=0x07;
UA[3]=0xFF; // aplicacao do xor ^
UA[4]=0x7E;
*/




#endif
