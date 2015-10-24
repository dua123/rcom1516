	

    /*Non-Canonical Input Processing*/
     
    #include "global.h"
    #include "filefunc.h"
     
    volatile int STOP=FALSE; // flag dos alarmes llopen
    int fd,c, res;
    char buf[5];
     
    void atende();
    int llopen();
    int llclose(); 
    void Test_a_Lot();

    int main(int argc, char** argv)
    {


    struct termios oldtio,newtio;
       
    if ( (argc < 2) 
        || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0))
        )
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
        leitura do(s) próximo(s) caracter(es)
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
	else		
		printf("llopen(): SUCESSO \n");

	if (llclose() == 1)
		printf("Falhou \n");
	else		
		printf("llcose(): SUCESSO \n");
           


            //Test_a_Lot();
           
     
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
            STOP = FALSE;
           
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
                    if (pak[0] == DISC[0] && pak[1] == DISC[1] && pak[2] == DISC[2] && pak[3] == DISC[3] && pak[4] == DISC[4])
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
     

     
    void Test_a_Lot(){
           
           
	//---------------------------------------------------
	char test[256]= {0x1A,0x2D,0x7E,0x5E,0x7D,0x4B,0x7D,0x5D}, text[513]={}, tempt[256]={};

	byte_stuffing_encode(test, text);
	de_stuffing(text,tempt);

	if(strcmp(test, tempt) == 0)
	{
	    printf("\n\n Byte Stuffing e decoding bem sucedido\n");
	}

	char buf_ficheiro[BUFFLENGTH];
	char buf_resultado[BUFFLENGTH];
	long file_size = file_to_buffer(buf_ficheiro, "image1.jpg");
	if (file_size == -1)
	{
	  perror("file_to_buffer()");
	exit(-1);
}
     
	int progress = 0;
	char chunk[256];
	while (progress < file_size)
	{
            progress += get_chunk(chunk, buf_ficheiro, 256, progress, file_size);
           
            if ( buffer_to_file(chunk, "image2.jpg", 256) == -1) {
            perror("buffer_to_file()");
            exit(-1);
            }
     
            printf("progress: %d", progress);
     
        }
           
       file_to_buffer(buf_resultado, "image2.jpg");
       
       if ( memcmp(buf_ficheiro, buf_resultado, file_size) == 0)
                    printf("SUCESSO");
            //---------------------------------------------------
           
    }

