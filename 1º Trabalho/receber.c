/*Non-Canonical Input Processing*/
 
#include "filefunc.h"
 
volatile int STOP=FALSE;
int c, res, timeouts = 0;
char buf[5];
void atende();
     
     
    int main(int argc, char** argv)
    {      

    //init
    init(argc, argv);
     

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
     
           
            STOP = FALSE;
            return 0;
     }
     
    int llclose(){
            //RECEBER DISC
            unsigned char * receive = DISC;
            unsigned char pak[5];
            char stopme = 0;
           
            while (stopme == 0) {  
                    usleep(50);
                    read(fd,&pak,5);
                   
                    if (pak[0] == DISC[0] && pak[1] == DISC[1] && pak[2] == DISC[2] && pak[3] == DISC[3] && pak[4] == DISC[4])
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
							printf("%2x, %2x, %2x, %2x, %2x\n", pak[0],pak[1],pak[2],pak[3],pak[4]);
printf("%2x, %2x, %2x, %2x, %2x\n", UA[0],UA[1],UA[2],UA[3],UA[4]);
                            return 1;
                    }
            }
           
            STOP = FALSE;
            return 0;
           
    }
         void atende()                   // atende alarme
    {
            if (STOP == FALSE && timeouts < 5)
            {
                    printf("llclose(): Ocorreu time out, re-enviar DISC \n");
                    timeouts++;
                    res = write(fd,buf,5);
                    alarm(3);
            } else
            {
                timeouts = 0;
            }
    }

     
 
