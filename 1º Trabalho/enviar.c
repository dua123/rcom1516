/*Non-Canonical Input Processing*/
 
#include "global.h"


volatile int STOP=FALSE;
int fd,c, res;
char buf[5];
 
void atende();

int main(int argc, char** argv)
{
SET[0]=0x7E;
SET[1]=0x03;
SET[2]=0x03;
SET[3]=0x00;
SET[4]=0x7E;

UA[0]=0x7E;
UA[1]=0x03;
UA[2]=0x07;
UA[3]=0xFF; // aplicacao do xor ^
UA[4]=0x7E;

    struct termios oldtio,newtio;
    int i, sum = 0, speed = 0;
   
    if ( (argc < 2) ||
             ((strcmp("/dev/ttyS0", argv[1])!=0) &&
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
 	 // instala  rotina que atende interrupcao
    printf("New termios structure set\n");
 
    /*testing*/
    printf("escrever Evia SET: \n");
    buf[0]=SET[0];
    buf[1]=SET[1];
    buf[2]=SET[2];
    buf[3]=SET[3];
    buf[4]=SET[4];
(void) signal(SIGALRM, atende); 
    printf("escrever %x: \n",buf[0]);
	while(STOP==FALSE)
	{
  		alarm(3); 
    		res = write(fd,buf,5);
		printf("%d bytes written\n", res);

	
  		//RECEBER SET
  		printf("Vou esperar pelo UA \n");
		unsigned char pak[5];
		usleep(50);
		
		res = read(fd,&pak,5);
		
		printf("%d bytes read\n", res);
		printf("Received Package: %x \n", pak[0]);
		if (pak[0] == UA[0] && pak[1] == UA[1] && pak[2] == UA[2] && pak[3] == UA[3] && pak[4] == UA[4])
		{
			printf("Sucesso \n");
			STOP = TRUE;
		}
		else
			printf("Falhou \n");
		
	}
	
/* 
	while (STOP==FALSE) {        loop for input 
	res = read(fd,buf,1);
      buf[res]=0;               
      printf(":%s:%d\n", buf, res);
      if (buf[0]==0) STOP=TRUE;
    }
*/



  /*
    O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar
    o indicado no guião
  */
 

 
   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
 
 
 
 
    close(fd);
    return 0;
}


void atende()                   // atende alarme
{
	if (STOP == FALSE)
	{
		printf("Ocorreu time out, re-enviar SET \n");
		res = write(fd,buf,5);
		alarm(3); 
	}
}
