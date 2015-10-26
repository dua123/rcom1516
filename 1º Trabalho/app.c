/*Non-Canonical Input Processing*/
 
#include "filefunc.h"

int user;
int proccess_arguments(int argc, char** argv);
     
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


	return 0;
}


 
int proccess_arguments(int argc, char** argv)
{

    if (    (argc < 3) ||
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
        user = EMISSOR;
    else if ( strcmp(argv[2], "recetor") == 0 )
        user = RECETOR;
    else
    {
        printf("O terceiro argumento deve ser 'emissor' OU 'recetor'\n");
        return -1;
    }


    if(user == EMISSOR)
    {
        if (argc != 4)
        {
            printf("E necessario um quarto argumento para o emissor:\n\t./app /dev/ttyS4 emissor filename\n");
            exit(1);
        }
        strcpy(filename,argv[3]);
        FILE * image_fd = fopen(argv[3], "r");
        if (image_fd == NULL)
        {
            printf("File doesn't exist!\n");
            return -1;
        }
        fclose(image_fd);
    }
    printf("User: %d\n", user);

    return 0;

}