CC = gcc 
CFLAGS = -Wall -o
LIBRARIES = global.h filefunc.h
FILES =  filefunc.c 

emissor: enviar.c $(FILES) $(LIBRARIES) 
	$(CC) enviar.c  $(FILES) $(CFLAGS) enviar 
recetor: receber.c $(FILES) $(LIBRARIES) 
	$(CC) receber.c $(FILES) $(CFLAGS) receber


