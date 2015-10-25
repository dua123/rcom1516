#include "segundo.h"


void setup()
{
	strcpy(str_hello, "Hello, ");
	strcpy(str_world, "World!");
}

void print_func()
{
	printf("%s\n", WORLD_CONST);
	//printf("%s\n", str_hello);
}


int fazer_trama_supervisao(char * res, int type, int direction, int r_num)
{
	res[0] = FLAG;
	if (direction == EMISSOR)
		res[1] = AE;
	else
		res[1] = AR;
	switch(type)
	{
	case TYPE_SET:
		res[2] = CSET;
		break;
	case TYPE_DISC:
		res[2] = CDISC;
		break;
	case TYPE_UA:
		res[2] = CUA;
		break;
	case TYPE_RR:
		res[2] = CRR(r_num);
		break;
	case TYPE_REJ:
		res[2] = CREJ(r_num);
		break;
	default:
		return -1;
		break;
	}

	res[3] = (char) (res[1] ^ res[2] );

	res[4] = FLAG;

	return 0;

}