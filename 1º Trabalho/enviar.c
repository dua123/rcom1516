/*Non-Canonical Input Processing*/
     
#include "filefunc.h"
int c, res;
char buf[FRAME_MAXSIZE];
 
void atende();
int llopen2();
int llclose();

int main(int argc, char** argv)
{
		
		    

    if (	(argc < 2) ||
            ((strcmp("/dev/ttyS0", argv[1])!=0) &&
            (strcmp("/dev/ttyS1", argv[1])!=0) &&
            (strcmp("/dev/ttyS2", argv[1])!=0) &&
            (strcmp("/dev/ttyS3", argv[1])!=0) &&
            (strcmp("/dev/ttyS4", argv[1])!=0) )
        ) 
    {
          printf("Usage:\tnserial SerialPort\n\tex: app /dev/ttyS4\n");
          exit(1);
    }
		
    //init
    init(argc, argv);
     
	if (llopen( EMISSOR ) == -1)
		printf("llopen():Falhou \n");
	else		
		printf("llopen(): SUCESSO \n");
	
/*
	if (llclose() == 1)
		printf("llclose():Falhou \n");
	else		
		printf("llcose(): SUCESSO \n"); 

	if (llread(EMISSOR) == 1)
		printf("llread(): Falhou \n");
	else		
		printf("llread(): SUCESSO \n"); 
*/

	if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
	  perror("tcsetattr");
	  exit(-1);
	}

	close(fd);
	return 0;
}
     
int llopen2()
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
			printf("llopen(): %d bytes written on fd: %d\n", res, fd);

			//RECEBER SET
			printf("llopen(): Vou esperar por UA \n");
			unsigned char pak[5];
			usleep(50);
			res = read(fd,&pak,5);
			printf("llopen(): %d bytes read\n", res);
			if ((char)pak[0] == (char)UA[0] && (char)pak[1] == (char)UA[1] && (char)pak[2] == (char)UA[2] && (char)pak[3] == (char)UA[3] && (char)pak[4] == (char)UA[4])
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
	STOP = FALSE;
   
	return 0;      
}
     
void atende()
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
            printf("llclose(): A enviar DISC: \n");
            buf[0]=DISC[0];
            buf[1]=DISC[1];
            buf[2]=DISC[2];
            buf[3]=DISC[3];
            buf[4]=DISC[4];
            (void) signal(SIGALRM, atende);
           
            while(STOP==FALSE)
            {
                    alarm(3);
            res = write(fd,buf,5);
                    printf("llclose(): %d bytes written\n", res);
     
                    //RECEBER DISC
                    printf("llclose(): Vou esperar por DISC \n");
                    unsigned char pak[5];
                    usleep(50);
                    res = read(fd,&pak,5);
                    printf("llclose(): %d bytes read\n", res);
                    if ((char) pak[0] == (char) DISC[0] && (char) pak[1] == (char)DISC[1] && (char)pak[2] == (char)DISC[2] && (char)pak[3] == (char)DISC[3] && (char)pak[4] == (char)DISC[4])
                    {
                            printf("llclose(): Recebi DISC, vou enviar UA \n");
                           
                            STOP = TRUE;
                    }
                    else
                    {
                            return 1;
                    }
                   
                    buf[0]=UA[0];
                    buf[1]=UA[1];
                    buf[2]=UA[2];
                    buf[3]=UA[3];
                    buf[4]=UA[4];
                    res = write(fd,buf,5);
                    printf("llclose(): %d bytes written\n", res);
                    sleep(3);      
            }
           
           
           
            STOP = FALSE;
            return 0;
    }



