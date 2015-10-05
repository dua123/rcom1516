/*Non-Canonical Input Processing*/



#include "global.h"



volatile int STOP=FALSE;

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

    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

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


	//RECEBER SET
	unsigned char * receive = SET;
	unsigned char pak;
	int state = 0; 
	while (STOP==FALSE) {
		printf("Current State: %d \n", state);
		usleep(50);
		read(fd,&pak,1);
		printf("Received Package: %x \n", pak);
		

		switch (state)
		{
		case 0: //Espera FLAG - F
			if (pak == SET[0])
			{
				printf("Passei ao estado 1 \n");
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
				printf("Recebi o SET inteiro\n");
				state = 0;
				STOP = TRUE;
			}
			else
				state = 0;
			break;
		}
	}

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
