/*Non-Canonical Input Processing*/
 
#include "filefunc.h"
     
     
int main(int argc, char** argv)
{      

	if (	(argc < 2) ||
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
	
    //init
    init(argc, argv);
     

    if(llopen( RECETOR ) == 1)
        printf("llopen(): Falhou\n");
    else
    	printf("llopen(): SUCESSO\n");


    if (llclose( RECETOR ) == 1)
		printf("llclose():Falhou \n");
	else		
		printf("llcose(): SUCESSO \n"); 

/*	
	if (llread(RECETOR) == 1)
		printf("llread(): Falhou \n");
	else		
		printf("llread(): SUCESSO \n"); 

*/	

	return 0;
}


 
