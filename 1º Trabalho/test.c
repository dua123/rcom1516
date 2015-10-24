#include "filefunc.h"


int main(){

	char data[DATAMAXSIZE];
	char unpak[DATAMAXSIZE];
	int i = 0;for(; i < 280; i++)
		data[i] = 0x11;
	char res[516];
	if (packup_data(res, 5, data, 280) == -1)
		printf("packup_data(): FALHOU!");
	if (unpack_data(unpak, 5, res) == -1)
		printf("unpack_data(): FALHOU!");
	if ( memcmp(data, unpak, 280) == 0)
		printf("Empacotamento e desempacotamento de dados bem sucedidos! \n");

	char res2[516];for(; i < 280; i++)
		data[i] = 0x00;
	if (packup_control(res2, 1, 500, "Hello, World!") == -1)
		printf("packup_control(): FALHOU!");

	char nome[128]; int n_pacotes;
	if ( (n_pacotes = unpack_control(res2, 1, nome)) == -1)
		printf("unpack_control(): FALHOU!");
	
	printf("Empacotamento e desempacotamento de comandos bem sucedidos! \n nome: %s, n_pacotes: %d\n", nome, n_pacotes);

	return 0;
}