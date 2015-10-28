#include "appfunc.h"

int proccess_arguments(int argc, char** argv){

    if ((argc < 3) ||
        ((strcmp("/dev/ttyS0", argv[1])!=0) &&
        (strcmp("/dev/ttyS1", argv[1])!=0) &&
        (strcmp("/dev/ttyS2", argv[1])!=0) &&
        (strcmp("/dev/ttyS3", argv[1])!=0) &&
        (strcmp("/dev/ttyS4", argv[1])!=0) )
        ) 
    {
        printf("Usage:\tnserial SerialPort\n\tex: ./app /dev/ttyS4 user\n");
        exit(1);
    }
    if ( strcmp(argv[2], "emissor") == 0 )
        Appdata.user = EMISSOR;
    else if ( strcmp(argv[2], "recetor") == 0 )
        Appdata.user = RECETOR;
    else
    {
        printf("O terceiro argumento deve ser 'emissor' OU 'recetor'\n");
        return -1;
    }
    if(Appdata.user == EMISSOR)
    {
        if (argc != 4)
        {
            printf("E necessario um quarto argumento para o emissor:\n\t./app /dev/ttyS4 emissor filename\n");
            exit(1);
        }
        strcpy(Appdata.filename,argv[3]);
        Appdata.fileDescriptor = fopen(argv[3], "r");
        if (Appdata.fileDescriptor == NULL)
        {
            printf("File doesn't exist!\n");
            return -1;
        }

    }
    printf("User: %d\n", Appdata.user);
    return 0;
}
void init(int argc, char** argv){ 
    Appdata.porta = open(argv[1], O_RDWR | O_NOCTTY );
    if (Appdata.porta <0) 
    {
        perror(argv[1]); exit(-1); 
    }
 
    if ( tcgetattr(Appdata.porta,&oldtio) == -1)
    { 
      perror("tcgetattr");
      exit(-1);
    }


    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME]    = 0; 
    newtio.c_cc[VMIN]     = 1; 


    tcflush(Appdata.porta, TCIOFLUSH);
 
    if ( tcsetattr(Appdata.porta,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
    printf("New termios structure set\n");
}
void finalize(){
    usleep(50);
    if ( tcsetattr(Appdata.porta,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
    if (Appdata.user == EMISSOR)   
        fclose(Appdata.fileDescriptor);
    close(Appdata.porta);
}

/*

long file_byte_size(char * name)
{
    FILE * fp = fopen(name, "r");
    long file_size;

    if (fp != NULL)
    {
        //obtain file size
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
    }
    fclose(fp);

    return file_size;
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
    return file_size;
}
int buffer_to_file(char * buffer, char * name, long file_size)
{
    FILE * fp = fopen(name, "a+");

    fwrite(buffer, sizeof(char), file_size, fp);

    fclose(fp);

    return 0;
}
int get_chunk(char * res, char * file_name, int chunk_size, int offset, long file_size)
{   
    FILE * fp = fopen(file_name, "r");
    fseek(fp, offset, SEEK_SET);
    size_t amount = chunk_size;
    if (fread(res, sizeof(char), amount, fp) == 0 )
        printf("file_to_buffer(): Erro a ler ficheiro \n");

    fclose(fp);
    return amount;
}
int packup_data(char * res, int n_seq, char * data, int data_size)
{
    if(data_size > 256 )
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
int packup_control(char * res, int command, unsigned int pack_amount, char * file_name)
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

    memcpy(&res[7], file_name, strlen(file_name));

    res[7 +  strlen(file_name) ] = 0x00;
    res[8 +  strlen(file_name) ] = 0x00;


    return (7 + strlen(file_name));
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

    memcpy(file_name, &pak[7], str_length);


    if (strlen(file_name) != str_length)
    {
        printf("unpack_control(): String and its length don't match\n");
        return -1;
    }

    return pack_amount;
}

*/