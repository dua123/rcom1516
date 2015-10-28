#include "appfunc.h"
#include "linkfunc.h"



     
int main(int argc, char** argv)
{      

	if (proccess_arguments(argc, argv) != 0)
        return -1;

    //init
    init(argc, argv);

    
    if(llopen( user ) == 1)
        printf("llopen(): Falhou\n");
    else
        printf("llopen(): SUCESSO\n");

    if (llread(user) == 1)
        printf("llread(): Falhou \n");
    else        
        printf("llread(): SUCESSO \n"); 

    
    if (llclose( user ) == 1)
		printf("llclose():Falhou \n");
	else		
		printf("llcose(): SUCESSO \n"); 
    
    finalize();
	return 0;
}

