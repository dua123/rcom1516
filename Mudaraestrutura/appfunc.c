
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


    Appdata.porta = 0;

    return 0;
}



int Logic_Emissor()
{
    //VER A QUANTIDADE DE TRAMAS DE DADOS A ENVIAR
    int total_file_size = file_byte_size();
    printf("bytes: %d, ", total_file_size);  
    if ((total_file_size % 256) > 0)
        Appdata.total_number_packets = (total_file_size / 256) + 1;
    else
        Appdata.total_number_packets = (total_file_size / 256);
    printf("chunks: %d\n", Appdata.total_number_packets);

    //MONTAR O COMANDO INCIAL
    int temp_size = packup_control(Appdata.pack_sent, PAK_CMD_FIRST);

    //ENVIAR A TRAMA DE INFORMACAO INICIAL
    llwrite(Appdata.fd_porta, Appdata.pack_sent, temp_size);
    
    
    //DADOS
    int progresso_do_envio;
    for (progresso_do_envio = 0; progresso_do_envio < Appdata.total_number_packets; progresso_do_envio++)
    {
        //Encontrar o proximo chunk
        char next_chunk[DATAMAXSIZE];
        get_chunk(next_chunk, DATAMAXSIZE, progresso_do_envio*DATAMAXSIZE);

        temp_size = packup_data(Appdata.pack_sent, progresso_do_envio, next_chunk, DATAMAXSIZE);
        
        llwrite(Appdata.fd_porta, Appdata.pack_sent, temp_size);        
    }

    //MONTAR O COMANDO FINAL
    temp_size = packup_control(Appdata.pack_sent, PAK_CMD_LAST);

    //ENVIAR O COMANDO FINAL
    llwrite(Appdata.fd_porta, Appdata.pack_sent, temp_size);    
    return 0;
}
int Logic_Recetor()
{

    //ESPERA PELA INFORMAÇAO DE NUMERO DE CHUNKS E DO NOME DO FICHEIRO
    int success = -1;
    while (success != 0)
    {
        success = 0;

        int size_read = llread(Appdata.fd_porta, Appdata.pack_received);
        if ( size_read == -1)
        {
            success = -1;
            printf("llread(): ERRO \n");    
        }

        //Verificaçoes
        Appdata.total_number_packets = unpack_control(Appdata.pack_received, PAK_CMD_FIRST, Appdata.filename);
        if(Appdata.total_number_packets == -1)
            success = -1;
        //printf("N pacotes: %d, Nome Ficheiro: %s\n", Appdata.total_number_packets, Appdata.filename);

        //Eviar resposta
        enviar_RR_REJ(success);
    }
    

    
    //TRANSFERENCIA DE DADOS
    int progresso_do_envio;
    for (progresso_do_envio = 0; progresso_do_envio < Appdata.total_number_packets; progresso_do_envio++)
    {
        //printf("Pacote %d\n", progresso_do_envio);
        printf("|"); fflush(stdout);
        success = -1;
        
        char dados_obtidos[DATAMAXSIZE];
        while (success != 0)
        {
            success = 0;
            int size_read = llread(Appdata.fd_porta, Appdata.pack_received);
            if ( size_read == -1)
            {
                success = -1;
                printf("llread(): ERRO \n");    
            }

            //Verificaçoes
            if ( unpack_data(dados_obtidos, progresso_do_envio, Appdata.pack_received) != 0)
                success = -1; 



            //Eviar resposta
            enviar_RR_REJ(success);
        }
        buffer_to_file(dados_obtidos, DATAMAXSIZE);
    }
    printf("\nCompleto \n");



    //ESPERA PELO COMANDO FINAL
    success = -1;
    while (success != 0)
    {
        success = 0;

        int size_read = llread(Appdata.fd_porta, Appdata.pack_received);
        if ( size_read == -1)
        {
            success = -1;
            printf("llread(): ERRO \n");    
        }

        //Verificaçoes
        Appdata.total_number_packets = unpack_control(Appdata.pack_received, PAK_CMD_LAST, Appdata.filename);
        if(Appdata.total_number_packets == -1)
            success = -1;   
        //printf("N pacotes: %d, Nome Ficheiro: %s\n", Appdata.total_number_packets, Appdata.filename);

        
        //Eviar resposta
        enviar_RR_REJ(success);
    }

    return 0;
}



long file_byte_size(char * name)
{
    FILE * fp = fopen(name, "r");
    long file_size;

    if (Appdata.fileDescriptor != NULL)
    {
        //obtain file size
        fseek(Appdata.fileDescriptor , 0, SEEK_END);
        file_size = ftell(Appdata.fileDescriptor );
        fseek(Appdata.fileDescriptor , 0, SEEK_SET);
    }

    return file_size;
}
int buffer_to_file(char * buffer, int buffersize)
{
    FILE * fp = fopen(Appdata.filename, "a+");

    fwrite(buffer, sizeof(char), buffersize, fp);

    fclose(fp);

    return 0;
}
int get_chunk(char * res, int chunk_size, int offset)
{
    fseek(Appdata.fileDescriptor, offset, SEEK_SET);
    size_t amount = chunk_size;
    if (fread(res, sizeof(char), amount, Appdata.fileDescriptor) == 0 )
        printf("file_to_buffer(): Erro a ler ficheiro \n");
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
int packup_control(char * res, int command)
{
    if (! (command == 1 || command == 2) )
    {
        printf("packup_control(): Invalid Command number, try 1 or 2\n");
        return -1;
    }
    res[0] = command;
    

    res[1] = 0; //T1 File size
    res[2] = 2; //T1 Amount of V1 octets
    res[3] = Appdata.total_number_packets / 256;
    res[4] = Appdata.total_number_packets % 256; //V1

    res[5] = 1; //T2 File size
    res[6] = strlen(Appdata.filename);

    memcpy(&res[7], Appdata.filename, strlen(Appdata.filename));

    res[7 +  strlen(Appdata.filename) ] = 0x00;
    res[8 +  strlen(Appdata.filename) ] = 0x00;


    return (7 + strlen(Appdata.filename));
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