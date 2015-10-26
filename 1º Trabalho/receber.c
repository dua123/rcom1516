/*Non-Canonical Input Processing*/
 
#include "filefunc.h"
 
int c, res, timeouts = 0;
char buf[FRAME_MAXSIZE];
int llopen2();
void atende();
     
     
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
     

    if(llopen2() == 1)
        printf("llopen(): Falhou\n");

/*	
	if (llread(RECETOR) == 1)
		printf("llread(): Falhou \n");
	else		
		printf("llread(): SUCESSO \n"); 

    if(llclose() == 1)
        printf("llclose(): Falhou\n");
*/	



    sleep(2);

      if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
          perror("tcsetattr");
          exit(-1);
        }
     
        close(fd);
        return 0;
}
     
int llopen2(){
     
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
			case 1: //Espera Edreço - A
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
					if (pak == (char)(SET[2]^SET[1]) )
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
	buf[3]=(UA[2]^UA[1]);
	buf[4]=UA[4];
	printf("llopen(): A enviar UA\n");
	res = write(fd,buf,5);
	printf("llopen(): %d bytes written\n", res);

   
	STOP = FALSE;
	return 0;
}
     
int llclose()
{
            //RECEBER DISC
            unsigned char * receive = DISC;
            unsigned char pak[5];
            char stopme = 0;
           
            while (stopme == 0) {  
                    usleep(50);
                    read(fd,&pak,5);
                   
                    if ((char)pak[0] == (char)DISC[0] && (char)pak[1] == (char)DISC[1] && (char)pak[2] == (char)DISC[2] && (char)pak[3] == (char)DISC[3] && (char)pak[4] == (char)DISC[4])
                    {
                            printf("llclose(): Recebi DISC, a re-enviar DISC \n");
                            stopme = 1;
                    }
            }
           
           
            buf[0]=DISC[0];
            buf[1]=DISC[1];
            buf[2]=DISC[2];
            buf[3]=DISC[3];
            buf[4]=DISC[4];
            (void) signal(SIGALRM, atende);

            usleep(50);
           
            while(STOP==FALSE)
            {
                    alarm(3);
                    res = write(fd,buf,5);
                    printf("llclose(): %d bytes written\n", res);
                   
                    //RECEBER UA
                    printf("llclose(): Vou esperar por UA \n");
                    usleep(50);
                    res = read(fd,&pak,5);
                    printf("llclose(): %d bytes read\n", res);
                    if ((char)pak[0] == (char)UA[0] && (char)pak[1] == (char)UA[1] && (char)pak[2] == (char)UA[2] && (char)pak[3] == (char)UA[3] && (char)pak[4] == (char)UA[4])
                    {
                            printf("llclose(): Recebi DISC, vou enviar UA \n");
                           
                            STOP = TRUE;
                    }
                    else
                    {

                            return 1;
                    }
            }
           
            STOP = FALSE;
            return 0;
           
}

	
void atende()	// atende alarme
{
	if (STOP == FALSE && timeouts < 5)
	{
			printf(" Ocorreu time out, re-enviar DISC \n");
			timeouts++;
			res = write(fd,buf,5);
			alarm(3);
	} else
	{
		timeouts = 0;
	}
}

    
 
