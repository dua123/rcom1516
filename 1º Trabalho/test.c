#include "filefunc.h"


int main(){

	char dados[5] = {0x01,0x02,0x03,0x04,0x05};
	char packet[DATAMAXSIZE];
	char stufpak[STUFFED_PACKET_MAXSIZE];
	char bcc2[1] = {0x00};
	char * frame;
	char * de_framed;
	char bcc2_res[1];
	char un_stuf[DATAMAXSIZE];
	char un_pak[DATAMAXSIZE];

	int tamanho = packup_data(packet, 0, dados, 5);
	if( tamanho == -1)
		printf("Falhou no packup_data()\n");

	int stuffcount = byte_stuffing_encode(packet, stufpak);
	if( stuffcount == -1)
		printf("Falhou no byte_stuffing_encode()\n");

	if( Fazer_trama(tamanho+stuffcount, stufpak, 0, frame, bcc2) == -1)
		printf("Falhou no Fazer_trama()\n");

	if( Desfazer_trama(frame, de_framed, 0, bcc2_res) == -1)
		printf("Falhou no Desfazer_trama() \n");
	if( memcmp (&bcc2, &bcc2_res, 1) != 0)
		printf("Falhou no Desfazer_trama(), bcc's \n");
		free(frame);



	//if( == -1)
	//	printf("Falhou no \n");

	//if( == -1)
	//	printf("Falhou no \n");

	//if( == -1)
	//	printf("Falhou no \n");
	
	printf("SUCESSO! \n");
	return 0;
}