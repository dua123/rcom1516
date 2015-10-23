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

#define EMISSOR 0
#define RECETOR 1

#define BUFFLENGTH 1000000

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */


#define FLAG 0x7e
#define AE 0x03
#define AR 0x01
#define CSET 0x07
#define CDISC 0x0b
#define CUA 0x03
#define BCCR 0x00
#define BCCE 0xFF
//#define CRR 0x21 ou 0x01
//#define CREJ 0x25 ou 0x05



unsigned char SET[5]={FLAG, AE, CSET ,AR,FLAG };
unsigned char UA[5]={FLAG,AE,AE,BCCE,FLAG};
unsigned char DISC[5]={FLAG,AE,CDISC,BCCR,FLAG};




#endif
