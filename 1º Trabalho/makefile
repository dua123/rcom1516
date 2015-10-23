CC = gcc 
CFLAGS = -o 
LIBRARIES = global.h

emissor: enviar.c recetor 
	$(CC) enviar.c  $(LIBRARIES) filefunc.c filefunc.h $(CFLAGS) enviar 
recetor: receber.c
	$(CC) receber.c $(LIBRARIES) filefunc.c filefunc.h $(CFLAGS) receber


