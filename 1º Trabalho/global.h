#ifndef HEADER_FILE
#define HEADER_FILE

#include <sys/types.h>	
#include <sys/stat.h>	
#include <fcntl.h>
#include <stdio.h>		
#include <unistd.h>		
#include <termios.h>
#include <stdlib.h>		
#include <string.h>		
#include <strings.h>	
#include <signal.h>		
#include <errno.h>		
#include <stdint.h>



#define FALSE 0
#define TRUE 1

#define EMISSOR 0
#define RECETOR 1

#define BUFFLENGTH 1000000

#define DATAMAXSIZE 256
#define PACKETMAXSIZE 260
#define STUFFED_PACKET_MAXSIZE 520
#define FRAME_MAXSIZE 526

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */


#define FLAG 	0x7e
#define AE 		0x03
#define AR 		0x01
#define CSET 	0x07
#define CDISC 	0x0b
#define CUA 	0x03
#define BCCR 	0x00
#define BCCE 	0xFF
#define CRR(r) ((r << 5 )|1)
#define CREJ(r) ((r << 5 )|5)
#define CDATA(s) (s << 5)

#define TYPE_SET 	1
#define TYPE_DISC 	2
#define TYPE_UA 	3
#define TYPE_RR 	4
#define TYPE_REJ 	5



static char SET[5]={FLAG, AE, CSET ,AR,FLAG };
static char UA[5]={FLAG,AE,AE,BCCE,FLAG};
static char DISC[5]={FLAG,AE,CDISC,BCCR,FLAG};

extern int fd;
extern struct termios oldtio,newtio;





#endif


