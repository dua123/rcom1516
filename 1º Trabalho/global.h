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

#define DATAMAXSIZE 512

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
#define CRR(r) ((r << 5 )|1)
#define CREJ(r) ((r << 5 )|5)


#define MAX_SIZE 513
static char SET[5]={FLAG, AE, CSET ,AR,FLAG };
static char UA[5]={FLAG,AE,AE,BCCE,FLAG};
static char DISC[5]={FLAG,AE,CDISC,BCCR,FLAG};

extern int fd;
extern struct termios oldtio,newtio;

struct applicationLayer {
	int fileDescriptor;/*Descritor correspondente à porta série*/
	int status; /*TRANSMITTER | RECEIVER*/
};

struct linkLayer {
	char port[20]; /*Dispositivo /dev/ttySx, x = 0, 1*/
	int baudRate; /*Velocidade de transmissão*/
	unsigned int sequenceNumber;   /*Número de sequência da trama: 0, 1*/
	unsigned int timeout; /*Valor do temporizador: 1 s*/
	unsigned int numTransmissions; /*Número de tentativas em caso de falha*/
	char frame[MAX_SIZE]; /*Trama**/
};


#endif


