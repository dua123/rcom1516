#ifndef LINKFUNC
#define LINKFUNC


#include "global.h"


#define EMISSOR 0
#define RECETOR 1

#define FALSE 0
#define TRUE 1


struct linkLayer{
	char port[20];
	int baudRate;
	unsigned int n_seq;
	unsigned int timeout;
	unsigned int numTransmissions;
	char frame[526];
} Linkdata;


//int llopen(int app);
//int llwrite(int app, char * buffer, int length);
//int llread(int app);
//int llclose(int app);


//int fazer_trama_supervisao(char * res, int type, int direction, int r_num);
//int fazer_trama_resposta(char * res, char * msg);
//int espera_e_responde_superv(char * msg, char * res);
//int envia_e_espera_superv(char * msg, char * res);

//void timeout();

//int byte_stuffing_encode(char * trama, char * res, int size);
//int de_stuffing(char * trama,char * res, int size);

//int Fazer_trama(int tamanho_dados, char * dados, int controlo, char * res, char * bcc2);
//int Desfazer_trama(char *dados, char * res, int controlo, char * bcc2);

//int espera_e_responde_dados(int type, int s, int n_seq, char * dados_obtidos);
//int envia_e_espera_dados(char * dados, int size, int s);


#endif
