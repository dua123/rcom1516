CC = gcc 
CFLAGS = -o 
LIBRARIES = global.h

emissor: enviar.c recetor
	$(CC) enviar.c $(LIBRARIES) $(CFLAGS) enviar 
recetor: receber.c 
	$(CC) receber.c $(LIBRARIES) $(CFLAGS) receber
