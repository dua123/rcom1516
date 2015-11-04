#ifndef LINKFUNC
#define LINKFUNC

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

#define _POSIX_SOURCE 1

#define BAUDRATE B38400

#define EMISSOR 0
#define RECETOR 1

#define FALSE 0
#define TRUE 1

#define DATAMAXSIZE 256
#define PACKETMAXSIZE 260
#define STUFFED_PACKET_MAXSIZE 520
#define FRAME_MAXSIZE 526

#define FLAG 	0x7e
#define AE 		0x03
#define AR 		0x01
#define CSET 	0x07
#define CDISC 	0x0b
#define CUA 	0x03
#define CRR(r) ((r << 5 )|1)
#define CREJ(r) ((r << 5 )|5)
#define CDATA(s) (s << 5)

#define TYPE_SET 	1
#define TYPE_DISC 	2
#define TYPE_UA 	3
#define TYPE_RR 	4
#define TYPE_REJ 	5


static char SET[5]=	{FLAG,AE,CSET,	0,FLAG};
static char UA[5]=	{FLAG,AE,CUA,	0,FLAG};
static char DISC[5]={FLAG,AE,CDISC,	0,FLAG};


struct linkLayer{
	int baudRate;
	unsigned int n_seq;
	unsigned int timeout;
	unsigned int numTransmissions;
	
	char frame_envio[526];
	char frame_resposta[526];
	int frame_size;

	int portfd;
	int user;

	int ALTERNATING;
};

struct termios oldtio,newtio;

int initialize(int port);
void finalize(int port_fd);
int fazer_trama_supervisao(char * res, int type, int direction, int r_num);
int fazer_trama_resposta(char * res, char * msg);
int espera_e_responde_superv(int port, char * msg, char * res);
int envia_e_espera_superv(int port, char * msg, char * res);

int envia_e_espera_dados(int size);

int espera_dados();


	void enviar_RR_REJ(int successo);

void timeout();
void timeout_data();

int byte_stuffing_encode(char * trama, char * res, int size);
int de_stuffing(char * trama,char * res, int size);

int Fazer_trama(int tamanho_dados, char * dados, char * res, char bcc2);
int Desfazer_trama(char *dados, char * res, int controlo);




#endif
