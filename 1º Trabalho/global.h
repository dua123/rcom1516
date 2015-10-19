#ifndef HEADER_FILE
#define HEADER_FILE


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define FALSE 0
#define TRUE 1

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */


unsigned char SET[5];
unsigned char UA[5];
unsigned char DISC[5];




#endif
