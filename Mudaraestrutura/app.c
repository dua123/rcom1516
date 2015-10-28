#include "linkfunc.h"
#include "appfunc.h"
     
int main(int argc, char** argv)
{
    printf("\e[1;1H\e[2J");

	if (proccess_arguments(argc, argv) != 0)
        return -1;
    init(argc, argv);


/*
    if(llopen( Appdata.status ) == 1)
        printf("llopen(): Falhou\n");
    else
        printf("llopen(): SUCESSO\n");

*/
    

 
    /*
    if (llread( Appdata.status) == 1)
        printf("llread(): Falhou \n");
    else        
        printf("llread(): SUCESSO \n"); 


    if (llclose( Appdata.status ) == 1)
		printf("llclose():Falhou \n");
	else		
		printf("llclose(): SUCESSO \n"); 
    
    */
    


    finalize();
	return 0;
}

