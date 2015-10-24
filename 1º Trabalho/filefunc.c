#include "filefunc.h"

int fd;
struct termios oldtio,newtio;

void init(int argc, char** argv)
{
    

    if (    (argc < 2) ||
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

    /*
        Open serial port device for reading and writing and not as controlling tty
        because we don't want to get killed if linenoise sends CTRL-C.
    */
 
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) 
    {
        perror(argv[1]); exit(-1); 
    }
 
    if ( tcgetattr(fd,&oldtio) == -1) /* save current port settings */
    { 
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

}



int byte_stuffing_encode(char * trama, char * res)
{
       
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
        printf("\n       %#X\n", res[j]);
 
       
        return 0;
}
 
int de_stuffing(char * trama,char * res)
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
 
long file_to_buffer(char * buffer, char * name)
{
    FILE * fp = fopen(name, "r");
    long file_size;

    if (fp != NULL)
    {
            //obtain file size
            fseek(fp, 0, SEEK_END);
            file_size = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            //Read the file
            if (fread(buffer, sizeof(char), file_size, fp) == 0 )
                    printf("file_to_buffer(): Erro a ler ficheiro \n");


            fclose(fp);


    }
    else
    {
            printf("file_to_buffer(): Erro a abrir ficheiro \n");
            return -1;
    }

    printf("file_to_buffer(): Terminou com sucesso \n");
    printf("file_to_buffer(): Tamanho do ficheiro: %lu \n", file_size);
    return file_size;
}
 
 
int buffer_to_file(char * buffer, char * name, long file_size)
{
    FILE * fp = fopen(name, "a+");

    fwrite(buffer, sizeof(char), file_size, fp);

    fclose(fp);

    return 0;
}
 
int get_chunk(char * res, char * buffer, int chunk_size, int offset, long file_size)
{
    int i = 0;
    for (; i < chunk_size && offset+i < file_size; i++)
    {
            res[i] = buffer[offset+i];

    }

    return i;
}

long packup_data(char * res, int n_seq, char * data, long data_size)
{
    if(data_size > 512 )
    {
        printf("packup_data(): Data read after stuffing was larger than maximum 512 byte\n");
        return -1;
    }

    int L1 = data_size % 256;
    int L2 = data_size / 256;

    int i = 0;
    for(i=0; i < 4; i++){
        switch (i)
        {   
        case 0:         //C
            res[i] = 0;
            break;
        case 1:         //N
            res[i] = n_seq;
            break;
        case 2:         //L1
            res[i] = L2;
            break;
        case 3:         //L2
            res[i] = L1;
            break;
        }
    }

    memcpy(&res[4], &data[0], data_size);
    return data_size+4;
}

long packup_control(char * res, int command, unsigned int pack_amount, char * file_name)
{
    if (! (command == 1 || command == 2) )
    {
        printf("packup_control(): Invalid Command number, try 1 or 2\n");
        return -1;
    }
    res[0] = command;
    

    res[1] = 0; //T1 File size
    res[2] = 2; //T1 Amount of V1 octets
    res[3] = pack_amount / 256;
    res[4] = pack_amount % 256; //V1

    res[5] = 1; //T2 File size
    res[6] = strlen(file_name);

    strcpy(&res[7], file_name);
    return 0;
}

int unpack_data(char * res, uint8_t n_seq, char * data)
{
    if(data[0] != 0x00)
    {
        printf("unpack_data(): This wasn't a Data Packet\n");
        return -1;
    }
    if(data[1] != n_seq)
    {
        printf("unpack_data(): Wrong sequence number\n");
        return -1;
    }

    int read_size = 256 * data[2]  + data[3];

    memcpy(&res[0], &data[4], read_size);

    return 0;
}

int unpack_control(char * pak, int command, char * file_name)
{

    if (command != pak[0] )
    {
        printf("unpack_control(): Wrong expected C value\n");
        return -1;
    }

    int pack_amount = 256 * (uint8_t) pak[3] + (uint8_t) pak[4];

    int str_length = (uint8_t) pak[6];
    strcpy(file_name, &pak[7]);

    if (strlen(file_name) != str_length)
    {
        printf("unpack_control(): String and its length don't match\n");
        return -1;
    }

    return pack_amount;
}
int Fazer_trama(int tamanho_dados, char * dados, int Controlo, char * res){
	
	int i =0;
	if(tamanho_dados>516)
		return -1;	
	res = malloc ((sizeof (char))*(tamanho_dados+6));
	res[0] = FLAG;
	//res[1] =
	//res[2]
	//res[3]
	//res[4]
	//res[5]
	for(i=0;i<tamanho_dados;i++)
		res[6+i] = dados[i];
	res[6+i]=FLAG;
	



}


