#ifndef LINKFUNC
#define LINKFUNC

#define _POSIX_SOURCE 1

#define DATAMAXSIZE 256
#define PACKETMAXSIZE 260

#define EMISSOR 0
#define RECETOR 1

int llopen(int port, int user);
int llclose(int port_fd);
int llwrite(int port_fd, char * message, int length);
int llread(int port_fd, char * message);

#endif
