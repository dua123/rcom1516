
#include "appfunc.h"
     
int main(int argc, char** argv)
{
    printf("\e[1;1H\e[2J");

	if (proccess_arguments(argc, argv) != 0)
        return -1;

    Appdata.fd_porta = llopen( Appdata.porta, Appdata.user );
    if( Appdata.fd_porta == 1)
        printf("llopen(): Falhou\n");
    else
        printf("llopen(): SUCESSO\n");
    

    if (Appdata.user == EMISSOR)
    {
        if (Logic_Emissor() == -1)
            printf("Emissor(): Falhou \n");
        else        
            printf("Emissor(): SUCESSO \n"); 
    }
    else
    {
        if (Logic_Recetor() == -1)
            printf("Recetor(): Falhou \n");
        else        
            printf("Recetor(): SUCESSO \n");
    }

    if (llclose( Appdata.fd_porta ) == -1)
		printf("llclose():Falhou \n");
	else		
		printf("llclose(): SUCESSO \n"); 
    
    
    if (Appdata.user == EMISSOR)   
        fclose(Appdata.fileDescriptor);

	return 0;
}

