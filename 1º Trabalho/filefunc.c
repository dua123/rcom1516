#include "filefunc.h"



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
    res[3] = (uint8_t) pack_amount / 256;
    res[4] = (uint8_t) pack_amount % 256; //V1
    
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

int unpack_control()
{

    return 0;
}
