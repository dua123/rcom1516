#ifndef APPFUNC
#define APPFUNC


#include "global.h"


int proccess_arguments(int argc, char** argv);
void init(int argc, char** argv);
void finalize();

long file_byte_size(char * name);
long file_to_buffer(char * buffer, char * name);
int buffer_to_file(char * buffer, char * name, long file_size);

int get_chunk(char * res, char * file_name, int chunk_size, int offset, long file_size);

int packup_control(char * res, int command, unsigned int pack_amount, char * file_name);
int unpack_control(char * pak, int command, char * file_name);
int packup_data(char * res, int n_seq, char * data, int data_size);
int unpack_data(char * res, uint8_t n_seq, char * data);


#endif
