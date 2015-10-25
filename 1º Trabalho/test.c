#include "filefunc.h"


int main(){

	char dados[10] = {0x00,0x01,0x06,0x07,0x08,0x7e,0x11,0xa0,0x7d,0x01};
	char packet[DATAMAXSIZE];
	char stufpak[STUFFED_PACKET_MAXSIZE];
	char bcc2[1] = {0x00};
	char frame[FRAME_MAXSIZE];
	char de_framed[FRAME_MAXSIZE];
	char bcc2_res[1];
	char un_stuf[DATAMAXSIZE];
	char un_pak[DATAMAXSIZE];

	int i;


		/*EMPACOTAR*/
	int tamanho = packup_data(packet, 1, dados, 10);
	printf("Tamanho apos empacotamento: %d\n", tamanho);
	if( tamanho == -1)
		printf("Falhou no packup_data()\n");

		/*STUFFING*/
	int stuffcount = byte_stuffing_encode(packet, stufpak, tamanho);
	printf("Tamanho acrescentado com stuffing: %d\n", stuffcount);
	if( stuffcount == -1)
		printf("Falhou no byte_stuffing_encode()\n");
	//for(i = 0; i < (tamanho+stuffcount); i++)
	//	printf("Novo: %2x, Original: %2x, comparison:%d \n", packet[i] ,stufpak[i] , memcmp (&packet[i], &stufpak[i], 1));


		/*FRAMING*/
	if( Fazer_trama(tamanho+stuffcount, stufpak, 0, frame, bcc2) == -1)
		{printf("Falhou no Fazer_trama()\n");	return -1;}

		/*DEFRAMING*/
	int tamanho_deframed = Desfazer_trama(frame, de_framed, 0, bcc2_res);
	printf("Tamanho apos remover trama: %d\n", tamanho_deframed);
	if( tamanho_deframed == -1)
		{printf("Falhou no Desfazer_trama() \n");return -1;}

	//for(i = 0; i < (tamanho_deframed); i++)
	//	printf("Novo: %2x, Original: %2x, comparison:%d \n", de_framed[i] ,stufpak[i] , memcmp (&de_framed[i], &stufpak[i], 1));

	if( memcmp (&bcc2, &bcc2_res, 1) != 0)	
		{printf("Falhou no Desfazer_trama(), BCC's \n");return -1;}
	if( memcmp (&de_framed, &stufpak, tamanho_deframed) != 0)	
		{printf("Falhou no Desfazer_trama(), de_framed \n");return -1;}

		/*DESTUFFING*/
	int tamanho_destuffed = de_stuffing(de_framed,un_stuf,tamanho_deframed);
	printf("Tamanho removido: %d\n", tamanho_destuffed);
	if( tamanho_destuffed == -1)
		printf("Falhou no de_stuffing()\n");

	
	for(i = 0; i < (tamanho_deframed-tamanho_destuffed); i++)
		printf("Unstuffed: %2x, Packet_Original: %2x, comparison:%d \n", un_stuf[i] ,packet[i] , memcmp (&un_stuf[i], &packet[i], 1));
	
	if( memcmp (&un_stuf, &packet, (tamanho_deframed-tamanho_destuffed)) != 0)	
		{printf("Falhou no de_stuffing(), memcmp \n");return -1;}

		/*UNPACKING*/



	//if( == -1)
	//	printf("Falhou no \n");
	//if( == -1)
	//	printf("Falhou no \n");
	
	printf("SUCESSO! \n");
	return 0;
}