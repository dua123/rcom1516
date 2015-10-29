#ifndef APPFUNC
#define APPFUNC


#include "linkfunc.h"

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


int proccess_arguments(int argc, char** argv);

int Logic_Emissor();
int Logic_Recetor();

long file_byte_size();
int buffer_to_file(char * buffer, int buffersize);
int get_chunk(char * res, int chunk_size, int offset);

int packup_control(char * res, int command);
int unpack_control(char * pak, int command, char * file_name);
int packup_data(char * res, int n_seq, char * data, int data_size);
int unpack_data(char * res, uint8_t n_seq, char * data);


#endif
