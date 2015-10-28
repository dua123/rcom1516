#ifndef APPFUNC
#define APPFUNC


#include "global.h"

#define FALSE 0
#define TRUE 1

#define EMISSOR 0
#define RECETOR 1

#define DATAMAXSIZE 256
#define PACKETMAXSIZE 260

#define PAK_CMD_FIRST	1
#define PAK_CMD_DATA	0
#define PAK_CMD_LAST	2

struct applicationLayer {
	char filename[255];
	FILE * fileDescriptor;

	int porta;
	int fd_porta;

	int user;
	int total_number_packets;

	char pack_sent[PACKETMAXSIZE];
	char pack_received[PACKETMAXSIZE];
} Appdata;


struct termios oldtio,newtio;


int proccess_arguments(int argc, char** argv);
void init(int argc, char** argv);
void finalize();

int Logic_Emissor();
int Logic_Recetor();

long file_byte_size();
//long file_to_buffer(char * buffer, char * name);
//int buffer_to_file(char * buffer, char * name, long file_size);

//int get_chunk(char * res, char * file_name, int chunk_size, int offset, long file_size);

int packup_control(char * res, int command);
//int unpack_control(char * pak, int command, char * file_name);
//int packup_data(char * res, int n_seq, char * data, int data_size);
//int unpack_data(char * res, uint8_t n_seq, char * data);


#endif
