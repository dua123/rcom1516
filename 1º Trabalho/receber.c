/*Non-Canonical Input Processing*/



#include "global.h"


volatile int STOP=FALSE;
int fd,c, res;
char buf[5];
int* de_stuffing(char * trama,char * res);


int main(int argc, char** argv)
{	
	SET[0]=0x7E;
	SET[1]=0x03;
	SET[2]=0x07;
	SET[3]=0x00;
	SET[4]=0x7E;

	UA[0]=0x7E;
	UA[1]=0x03;
	UA[2]=0x03;
	UA[3]=0xFF;
	UA[4]=0x7E;

	DISC[0]=0x7E;
	DISC[1]=0x03;
	DISC[2]=0x0B;
	DISC[3]=0x00;
	DISC[4]=0x7E;


	
    struct termios oldtio,newtio;


    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS2", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS3", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS4", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS4\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */


	/* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
	*/

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
	printf("New termios structure set\n");


	
	if(llopen() == 1)
		printf("llopen(): Falhou\n");
	
	if(llclose() == 1)
		printf("llclose(): Falhou\n");
	

	sleep(2);
	

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}

int llopen(){

	//RECEBER SET
	unsigned char * receive = SET;
	unsigned char pak;
	int state = 0; 
	while (STOP==FALSE) {
		usleep(50);
		read(fd,&pak,1);

		switch (state)
		{
		case 0: //Espera FLAG - F
			if (pak == SET[0])
			{
				state++;
			}
			break;
		case 1:	//Espera Edreço - A
			if (pak == SET[1])
				state++;
			else if (pak == SET[0])
				;
			else
				state = 0;
			break;
		case 2: // Espera Controlo - C
			if (pak == SET[2])
				state++;
			else if (pak == SET[0])
				state = 1;
			else
				state = 0;
			break;
		case 3: // Espera de BCC
			if (pak == SET[3])
				state++;
			else if (pak == SET[0])
				state = 1;
			else
				state = 0;
			break;
		case 4: // Espera Flag - F
			if (pak == SET[4])
			{
				printf("llopen(): Recebi o SET inteiro\n");
				state = 0;
				STOP = TRUE;
			}
			else
				state = 0;
			break;
		}
	}

	// Enviar UA resposta
	usleep(50);
	buf[0]=UA[0];
	buf[1]=UA[1];
	buf[2]=UA[2];
	buf[3]=UA[3];
	buf[4]=UA[4];
	printf("llopen(): A enviar UA\n");
	res = write(fd,buf,5);
	printf("llopen(): %d bytes written\n", res);

	return 0;
 }

int llclose(){
	STOP = FALSE;
	
	char test[513]= {0x1A,0x2D,0x7D,0x5E,0x5E,0x7D,0x5D,0x4B,0x7D,0x5D,0x5D,0x7E}, text[513]={};
	de_stuffing(test, text);
}

int* de_stuffing(char * trama,char * res)
{
	int i, j=0;	
	
	for(i = 0; i < strlen(trama); i++, j++)
	{
		printf("\n%#X", trama[i]);
		

		if (trama[i]  == 0x7D && trama[i+1] == 0x5E) 
		{
			res[j] = 0x7E;
                        printf("   %#X", res[j]);
			printf("\n%#X", trama[i+1]);
			i++;
		}
		else if(trama[i]  == 0x7D && trama[i+1] == 0x5D)
		{
			res[j] = 0x7D;
                        printf("   %#X", res[j]);
			printf("\n%#X", trama[i+1]);
			i++;
		}
		else if(trama[i] == 0x7E)
		{
		}
		else
		{
			res[j] = trama[i];
			printf("   %#X", res[j]);
		}

	}
	printf("\n");
	
	return 0;
}




