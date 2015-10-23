#ifndef FILEFUNC
#define FILEFUNC


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int byte_stuffing_encode(char * trama, char * res);
int de_stuffing(char * trama,char * res);
long file_to_buffer(char * buffer, char * name);
int buffer_to_file(char * buffer, char * name, long file_size);
int get_chunk(char * res, char * buffer, int chunk_size, int offset, long file_size);



#endif
