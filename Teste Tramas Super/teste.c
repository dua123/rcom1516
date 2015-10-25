#include "segundo.h"


int main()
{
	setup();
	printf("%s", HELLO_CONST);
	//printf("%s", str_hello);
	print_func();

	char buf[5];
	char res[5] = {FLAG, AE, CRR(0), AE ^ CRR(0), FLAG};

	if(fazer_trama_supervisao(buf, TYPE_RR, EMISSOR, 0) == -1)
	{
		printf("Welp!\n");
		return -1;
	} 
	if (memcmp(buf, res, 5) != 0)
	{
		printf("Welp!\n");
		return -1;
	} 
	else
		printf("Okay.\n");

	return 0;
}