/*Non-Canonical Input Processing*/
 
#include "global.h"


volatile int STOP=FALSE; // flag dos alarmes llopen
int fd,c, res;
char buf[5];
 
void atende();
int llopen();
int llclose();
int byte_stuffing_encode(char * trama, char * res);

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
	UA[3]=0xFF; // aplicacao do xor ^
	UA[4]=0x7E;

	DISC[0]=0x7E;
	DISC[1]=0x03;
	DISC[2]=0x0B;
	DISC[3]=0x00;
	DISC[4]=0x7E;

	struct termios oldtio,newtio;
	int i, sum = 0, speed = 0;
	   
	if ( (argc < 2) || (strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) ) 
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS4\n");
		exit(1);
	}
  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
 
 
	fd = open(argv[1], O_RDWR | O_NOCTTY );
	if (fd <0) 
	{
		perror(argv[1]); exit(-1); 
	}
 
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
    leitura do(s) pr�ximo(s) caracter(es)
  */
 
 
 
    tcflush(fd, TCIOFLUSH);
 
    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
 	 // instala  rotina que atende interrupcao
    printf("New termios structure set\n");
 
	if (llopen() == 1)
		printf("Falhou \n");

	if (llclose() == 1)
		printf("Falhou \n");

 
   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
 
 
 
 
    close(fd);
    return 0;
}

int llopen()
{
	printf("llopen(): A enviar SET: \n");
	buf[0]=SET[0];
	buf[1]=SET[1];
	buf[2]=SET[2];
	buf[3]=SET[3];
	buf[4]=SET[4];
	(void) signal(SIGALRM, atende); 
	while(STOP==FALSE)
	{
  		alarm(3); 
    		res = write(fd,buf,5);
		printf("llopen(): %d bytes written\n", res);

  		//RECEBER SET
  		printf("llopen(): Vou esperar por UA \n");
		unsigned char pak[5];
		usleep(50);
		res = read(fd,&pak,5);
		printf("llopen(): %d bytes read\n", res);
		if (pak[0] == UA[0] && pak[1] == UA[1] && pak[2] == UA[2] && pak[3] == UA[3] && pak[4] == UA[4])
		{
			printf("llopen(): Recebi UA \n");
			STOP = TRUE;
		}
		else
		{
			return 1;
		}
		sleep(2);
		
	}

	
	return 0;	
}

void atende()                   // atende alarme
{
	if (STOP == FALSE)
	{
		printf("llopen(): Ocorreu time out, re-enviar SET \n");
		res = write(fd,buf,5);
		alarm(3); 
	}
}

int llclose()
{
	STOP = FALSE;

	char test[256]= {0x1A,0x2D,0x7E,0x5E,0x7D,0x4B,0x7D,0x5D}, text[513]={};
	byte_stuffing_encode(test, text);
}

int byte_stuffing_encode(char * trama, char * res){
	
	int i, j=0;	
	
	for(i = 0; i < strlen(trama); i++, j++)
	{
		printf("\n%#X", trama[i]);
		

		if (trama[i]  == 0x7E) 
		{
			res[j] = 0x7D;
                        printf("   %#X", res[j]);
			j++;
			res[j] = 0x5E;
			printf("   %#X", res[j]);
		}
		else if(trama[i]  == 0x7D)
		{
			res[j] = 0x7D;
                        printf("   %#X", res[j]);
			j++;
			res[j] = 0x5D;
			printf("   %#X", res[j]);
		}
		else{
			res[j] = trama[i];
			printf("   %#X", res[j]);

		}

	}
	res[j] = 0x7E;
	printf("\n      %#X", res[j]);

	
	return 0;
}




