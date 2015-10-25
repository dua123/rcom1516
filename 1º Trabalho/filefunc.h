#ifndef FILEFUNC
#define FILEFUNC


#include "global.h"


void init(int argc, char** argv);

long file_to_buffer(char * buffer, char * name);
int buffer_to_file(char * buffer, char * name, long file_size);

int get_chunk(char * res, char * buffer, int chunk_size, int offset, long file_size);

int packup_control(char * res, int command, unsigned int pack_amount, char * file_name);
int unpack_control(char * pak, int command, char * file_name);
int packup_data(char * res, int n_seq, char * data, int data_size);
int unpack_data(char * res, uint8_t n_seq, char * data);

int byte_stuffing_encode(char * trama, char * res);
int de_stuffing(char * trama,char * res);

int Fazer_trama(int tamanho_dados, char * dados, int controlo, char * res, char * bcc2);
int Desfazer_trama(char *dados, char * res, int controlo, char * bcc2);

int fazer_trama_supervisao(char * res, int type, int direction, int r_num);


#endif
