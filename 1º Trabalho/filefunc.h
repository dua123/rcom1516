#ifndef FILEFUNC
#define FILEFUNC


#include "global.h"


void init(int argc, char** argv);
int byte_stuffing_encode(char * trama, char * res);
int de_stuffing(char * trama,char * res);
long file_to_buffer(char * buffer, char * name);
int buffer_to_file(char * buffer, char * name, long file_size);
int get_chunk(char * res, char * buffer, int chunk_size, int offset, long file_size);
long packup_data(char * res, int n_seq, char * data, long data_size);
long packup_control(char * res, int command, unsigned int pack_amount, char * file_name);
int unpack_data(char * res, uint8_t n_seq, char * data);
int unpack_control(char * pak, int command, char * file_name);


#endif
