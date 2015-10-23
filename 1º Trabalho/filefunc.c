#include"filefunc.h"

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


