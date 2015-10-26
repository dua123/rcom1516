#include "filefunc.h"


int fd;
char BCC_2;
char filename[48];
struct termios oldtio,newtio;
volatile int STOP=FALSE; // flag dos alarmes llopen
char Alarm_buffer[FRAME_MAXSIZE];
int total_number_packets;



void init(int argc, char** argv){ 
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) 
    {
        perror(argv[1]); exit(-1); 
    }
 
    if ( tcgetattr(fd,&oldtio) == -1) /* save current port settings */
    { 
      perror("tcgetattr");
      exit(-1);
    }


    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
 
    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
 
    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */
    /*
        VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
        leitura do(s) próximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);
 
    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");
}
void finalize(){
    sleep(2);
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
    close(fd);
}


int byte_stuffing_encode(char * trama, char * res, int size)
{
       
    int i, j=0; 
    int count = 0;   
   
    for(i = 0; i < size; i++, j++)
    {
            if (trama[i]  == 0x7E)
            {
                    res[j] = 0x7D;
                    j++; count++;
                    res[j] = 0x5E;
            }
            else if(trama[i]  == 0x7D)
            {
                    res[j] = 0x7D;
                    j++; count++;
                    res[j] = 0x5D;
            }
            else{
                   res[j] = trama[i];
                    
            }
    }
    return count;
}
int de_stuffing(char * trama,char * res, int size)
{
    int i, j=0;  
    int count = 0;  
   
    for(i = 0; i < size; i++, j++)
    {
        if (trama[i]  == 0x7D && trama[i+1] == 0x5E)
        {
                res[j] = 0x7E;
                i++; count++;
        }
        else if(trama[i]  == 0x7D && trama[i+1] == 0x5D)
        {
                res[j] = 0x7D;
                i++;count++;
        }
        else
        {
                res[j] = trama[i];
        }
    }
   
    return count;
}

long file_byte_size(char * name)
{
    FILE * fp = fopen(name, "r");
    long file_size;

    if (fp != NULL)
    {
        //obtain file size
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
    }
    fclose(fp);

    return file_size;
}
long file_to_buffer(char * buffer, char * name)
{
    FILE * fp = fopen(name, "r");
    long file_size;

    if (fp != NULL)
    {
        //obtain file size
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        //Read the file
        if (fread(buffer, sizeof(char), file_size, fp) == 0 )
                printf("file_to_buffer(): Erro a ler ficheiro \n");

        fclose(fp);


    }
    else
    {
        printf("file_to_buffer(): Erro a abrir ficheiro \n");
        return -1;
    }
    return file_size;
}
int buffer_to_file(char * buffer, char * name, long file_size)
{
    FILE * fp = fopen(name, "a+");

    fwrite(buffer, sizeof(char), file_size, fp);

    fclose(fp);

    return 0;
}
 
int get_chunk(char * res, char * file_name, int chunk_size, int offset, long file_size)
{   
    FILE * fp = fopen(file_name, "r");
    fseek(fp, offset, SEEK_SET);
    size_t amount = chunk_size;
    if (fread(res, sizeof(char), amount, fp) == 0 )
        printf("file_to_buffer(): Erro a ler ficheiro \n");

    fclose(fp);
    return amount;

}

int packup_data(char * res, int n_seq, char * data, int data_size)
{
    if(data_size > 256 )
    {
        printf("packup_data(): Data read after stuffing was larger than maximum 512 byte\n");
        return -1;
    }

    int L1 = data_size % 256;
    int L2 = data_size / 256;

    int i = 0;
    for(i=0; i < 4; i++){
        switch (i)
        {   
        case 0:         //C
            res[i] = 0;
            break;
        case 1:         //N
            res[i] = n_seq;
            break;
        case 2:         //L1
            res[i] = L2;
            break;
        case 3:         //L2
            res[i] = L1;
            break;
        }
    }

    memcpy(&res[4], &data[0], data_size);
    return data_size+4;
}
int packup_control(char * res, int command, unsigned int pack_amount, char * file_name)
{
    if (! (command == 1 || command == 2) )
    {
        printf("packup_control(): Invalid Command number, try 1 or 2\n");
        return -1;
    }
    res[0] = command;
    

    res[1] = 0; //T1 File size
    res[2] = 2; //T1 Amount of V1 octets
    res[3] = pack_amount / 256;
    res[4] = pack_amount % 256; //V1

    res[5] = 1; //T2 File size
    res[6] = strlen(file_name);

    memcpy(&res[7], file_name, strlen(file_name));

    res[7 +  strlen(file_name) ] = 0x00;
    res[8 +  strlen(file_name) ] = 0x00;


    return (7 + strlen(file_name));
}

int unpack_data(char * res, uint8_t n_seq, char * data)
{

    if(data[0] != 0x00)
    {
        printf("unpack_data(): This wasn't a Data Packet\n");
        return -1;
    }
    if(data[1] != n_seq)
    {
        printf("unpack_data(): Wrong sequence number\n");
        return -1;
    }

    int read_size = 256 * data[2]  + data[3];

    memcpy(&res[0], &data[4], read_size);

    return 0;
}

int unpack_control(char * pak, int command, char * file_name)
{

    if (command != pak[0] )
    {
        printf("unpack_control(): Wrong expected C value\n");
        return -1;
    }

    int pack_amount = 256 * (uint8_t) pak[3] + (uint8_t) pak[4];

    int str_length = (uint8_t) pak[6];

    memcpy(file_name, &pak[7], str_length);


    if (strlen(file_name) != str_length)
    {
        printf("unpack_control(): String and its length don't match\n");
        return -1;
    }

    return pack_amount;
}

int Fazer_trama(int tamanho_dados, char * dados, int controlo, char * res, char * bcc2){

	if(tamanho_dados> STUFFED_PACKET_MAXSIZE)
		return -1;	

	res[0] = FLAG;
	res[1] = AE;
	res[2] = CDATA(controlo);
	res[3] = (AE ^ CDATA(controlo));

    memcpy(&res[4], &dados[0], tamanho_dados);
    memcpy(&res[4+tamanho_dados],&bcc2, 1);
	res[5+tamanho_dados] =  FLAG;
	
	return 0;

}
int Desfazer_trama(char *dados, char * res, int controlo, char * bcc2){
	
if(dados[0]!= FLAG)
        return -1;
    if(dados[1]!= AE)
        return -1;
    if(dados[2]!= CDATA(controlo))
        return -1;
    if( dados[3] != (AE ^ CDATA(controlo)) )
    	return -1;



    int i = 0;

    while (dados[4+i] != FLAG)
    {
        i++;
        if (i > STUFFED_PACKET_MAXSIZE)  
        {
            printf("Erro no tamanho dos dados\n");
            return -1;
        }
    }

    memcpy(&res[0], &dados[4], i-1);
    memcpy(&bcc2, &dados[4+i-1], 1);

    if(dados[4+i]!= FLAG)
        return -1;

return i-1;

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
int fazer_trama_resposta(char * res, char * msg)
{
	if (msg[0] != FLAG)
		return -1;
    res[0] = msg[0];
    res[1] = msg[1];
    if (msg[2] == SET[2])
    {
        res[2] = UA[2];
        res[3] = (UA[2]^UA[1]);
    } else if (msg[2] == DISC[2])
    {
        res[2] = DISC[2];
        res[3] = (DISC[2]^DISC[1]);
    }
    else if (msg[2] == UA[2])
    {
        res[2] = UA[2];
        res[3] = (UA[2]^UA[1]);
    }
    res[4] = msg[4];

    return 0;
	
}


int test_file_chunking(char * source_filename, char * dest_filename){
    char buf_ficheiro[BUFFLENGTH];
    char buf_resultado[BUFFLENGTH];
    long file_size = file_to_buffer(buf_ficheiro, source_filename);
    if (file_size == -1)
    {
      perror("file_to_buffer()");
    exit(-1);
    }

    int progress = 0;
    char chunk[256];
    while (progress < file_size)
    {
        progress += get_chunk(chunk, buf_ficheiro, 256, progress, file_size);
       
        if ( buffer_to_file(chunk, dest_filename, 256) == -1) {
        perror("buffer_to_file()");
        exit(-1);
        }

        printf("progress: %d", progress);

    }
    file_to_buffer(buf_resultado, dest_filename);

    if ( memcmp(buf_ficheiro, buf_resultado, file_size) == 0)
                printf("SUCESSO");

    return 0;
}


int llopen(int app)
{

    //Construir Trama SET
    char SET_frame[5];
    fazer_trama_supervisao(SET_frame, TYPE_SET, EMISSOR, 0);

    char SET_resp[5];
    if (fazer_trama_resposta(SET_resp, SET_frame) == -1)
        return -1;


	if (app == EMISSOR)
	{
        int final = envia_e_espera_superv(SET_frame, SET_resp);
		return final;
    } 
    else if(app == RECETOR)
    {
        int final = espera_e_responde_superv(SET_frame, SET_resp);
        return final;
    }


    return -1;
}

int llclose(int app)
{
    //Construir Trama SET
    char DISC_frame[5];
    fazer_trama_supervisao(DISC_frame, TYPE_DISC, EMISSOR, 0);

    char DISC_resp[5];
    if (fazer_trama_resposta(DISC_resp, DISC_frame) == -1)
        return -1;

    int final = 0;
    if(app == EMISSOR)
    {
        final = envia_e_espera_superv(DISC_frame, DISC_resp);
        if (final != -1)
        {
            fazer_trama_supervisao(DISC_frame, TYPE_UA, EMISSOR, 0);
            write(fd,DISC_frame,5);
        }
        return final;
    }
    else if(app == RECETOR)
    {
        final = espera_e_responde_superv(DISC_frame, DISC_resp);
        if (final != -1)
        {
            fazer_trama_supervisao(DISC_frame, TYPE_UA, EMISSOR, 0);
            int i; final = 0; char pak;
            for (i = 0; i < 5; i++)
            {   
                read(fd,&pak,1);
                if (pak != (char)DISC_frame[i])
                    final = -1;
            }
        }
        
    }
    finalize();

    return final;   
}

int llread(int app)
{
    if(app == EMISSOR)
    {

        //VER A QUANTIDADE DE TRAMAS DE DADOS A ENVIAR
        int total_file_size = file_byte_size(filename);
        int total_number_packets;
        printf("bytes: %d, ", total_file_size);
        if ((total_file_size % 256) > 0)
            total_number_packets = (total_file_size / 256) + 1;
        else
            total_number_packets = (total_file_size / 256);
        printf("chunks: %d\n", total_number_packets);

	
        //MONTAR O COMANDO INCIAL
        char pack_command[PACKETMAXSIZE]; int ALTERNATING = 0;
        int temp_size = packup_control(pack_command, PAK_CMD_FIRST, total_number_packets, filename);

	int j = 0;
	for (j= 0; j<temp_size; j++ )
	{
		if (j == 0)
			BCC_2 = pack_command[j];
		else 
			BCC_2 = (BCC_2 ^ pack_command[j]);
	}

        //ENVIAR A TRAMA DE INFORMACAO INICIAL
        envia_e_espera_dados(pack_command, ALTERNATING, temp_size);
        ALTERNATING = 1;

        //DADOS
        int progresso_do_envio;
        for (progresso_do_envio = 0; progresso_do_envio < total_number_packets; progresso_do_envio++)
        {
            //Encontrar o proximo chunk
            char next_chunk[DATAMAXSIZE];
            get_chunk(next_chunk, filename, DATAMAXSIZE, progresso_do_envio*DATAMAXSIZE, total_file_size);
            char data_packet[PACKETMAXSIZE];
            temp_size = packup_data(data_packet, progresso_do_envio, next_chunk, DATAMAXSIZE);

		
		for (j= 0; j<temp_size; j++ )
		{
			if (j == 0)
				BCC_2 = data_packet[j];
			else 
				BCC_2 = (BCC_2 ^ data_packet[j]);
		}
            
            envia_e_espera_dados(data_packet, ALTERNATING, temp_size);
            if (ALTERNATING == 0) ALTERNATING = 1; else ALTERNATING = 0;
        }



        //MONTAR O COMANDO FINAL
 	temp_size = packup_control(pack_command, PAK_CMD_LAST, total_number_packets, filename);
	
	for (j= 0; j<temp_size; j++ )
	{
		if (j == 0)
			BCC_2 = pack_command[j];
		else 
			BCC_2 = (BCC_2 ^ pack_command[j]);
	}
        envia_e_espera_dados(pack_command, ALTERNATING, temp_size);
        if (ALTERNATING == 0) ALTERNATING = 1; else ALTERNATING = 0;
        return 0;
    }   
    else if(app == RECETOR)
    {

        //ESPERA PELA INFORMAÇAO DE NUMERO DE CHUNKS E DO NOME DO FICHEIRO
        int ALTERNATING = 0;
        int success = -1;
        char received_data[DATAMAXSIZE];
        while (success != 0)
            success = espera_e_responde_dados(PAK_CMD_FIRST, ALTERNATING, 0, received_data);
        ALTERNATING = 1;

        //dados
        int progresso_do_envio;
        for (progresso_do_envio = 0; progresso_do_envio < total_number_packets; progresso_do_envio++)
        {
            printf("|");
            success = -1;
            while (success != 0)
                success = espera_e_responde_dados(PAK_CMD_DATA, ALTERNATING, progresso_do_envio, received_data);
            if (ALTERNATING == 0) ALTERNATING = 1; else ALTERNATING = 0;

            buffer_to_file(received_data, filename, DATAMAXSIZE);
        }


        //ESPERA PELO COMANDO final
        success = -1;
        while (success != 0)
            success = espera_e_responde_dados(PAK_CMD_LAST, ALTERNATING, 0, received_data);
        if (ALTERNATING == 0) ALTERNATING = 1; else ALTERNATING = 0;


        return 0;
    }
    else
       return -1;
}

void timeout()                   // atende alarme
{
	if (STOP == FALSE)
	{
		printf("Ocorreu time out\n");
		write(fd,Alarm_buffer,5);
		alarm(1);
	}
}


int espera_e_responde_superv(char * msg, char * res)
{

    unsigned char pak;
    int state = 0;
    while (STOP==FALSE) 
    {
        usleep(50);
        read(fd,&pak,1);

        switch (state)
        {
        case 0: //Espera FLAG - F
                if (pak == msg[0])
                {
                        state++;
                }
                break;
        case 1: //Espera Edreço - A
                if (pak == msg[1])
                        state++;
                else if (pak == msg[0])
                        ;
                else
                        state = 0;
                break;
        case 2: // Espera Controlo - C
                if (pak == msg[2])
                        state++;
                else if (pak == msg[0])
                        state = 1;
                else
                        state = 0;
                break;
        case 3: // Espera de BCC
                if (pak == msg[3] )
                        state++;
                else if (pak == msg[0])
                        state = 1;
                else
                        state = 0;
                break;
        case 4: // Espera Flag - F
                if (pak == msg[4])
                {
                        STOP = TRUE;
                }
                else
                        state = 0;
                break;
        }
    }

    usleep(50);
    write(fd,res,5);    
    STOP = FALSE;
    return 0;
	
}
int envia_e_espera_superv(char * msg, char * res)
{
	//Preparar a função de timeout
	memcpy(&Alarm_buffer[0], &msg[0], 5);
	(void) signal(SIGALRM, timeout);
	alarm(2);

    //enviar
	write(fd,Alarm_buffer,5);
	
	//Ciclo de Espera
	unsigned char pak;
	
	
    int state = 0;
	while(STOP==FALSE)
	{		
		usleep(50);
		read(fd,&pak,1);
        switch (state)
		{
		case 0: //Espera FLAG - F
			if (pak == (char)res[0])
			{
				state++;
			}
			break;
		case 1: //Espera Edreço - A
			if (pak == (char)res[1])
				state++;
			else if (pak == (char)res[0])
				;
			else
				state = 0;
			break;
		case 2: // Espera Controlo - C
			if (pak == (char)res[2])
				state++;
			else if (pak == (char)res[0])
				state = 1;
			else
				state = 0;
			break;
		case 3: // Espera de BCC
			if (pak == (char)res[3])
				state++;
			else if (pak == (char)res[0])
				state = 1;
			else
				state = 0;
			break;
		case 4: // Espera Flag - F
			if (pak == (char)res[4])
			{
				state = 0;
				STOP = TRUE;
				usleep(50);
				STOP = FALSE;
				return 0;  
					
			}
			else
				state = 0;
			break;
		}
	}	
	
	return -1;
}

int espera_e_responde_dados(int type, int s, int n_seq, char * dados_obtidos){

    unsigned char pak;
    char incoming_frame[FRAME_MAXSIZE];
    int i = 0;
    int state = 0;

    while (STOP==FALSE) 
    {
        //printf("state: %d\n", state);
        usleep(5);
        read(fd,&pak,1);
        switch (state)
        {
        case 0: //Espera FLAG - F
                if (pak == FLAG)
                {   
                    incoming_frame[i] = pak;
                    i++;
                    state++;
                }
                break;
        case 1: //Espera Edreço - A
                if (pak == AE)
                {
                    incoming_frame[i] = pak;
                    i++;
                    state++;
                }       
                else if (pak == FLAG)
                    i = 1;
                else
                    state = 0;
                break;
        case 2: // Espera Controlo - C
                if (pak == CDATA(s))
                {
                    incoming_frame[i] = pak;
                    i++;
                    state++;
                }
                else if (pak == FLAG)
                        state = 1;
                else
                        state = 0;
                break;
        case 3: // Espera de BCC
                if (pak == (AE ^ CDATA(s)) )
                {
                    incoming_frame[i] = pak;
                    i++;
                    state++;
                }
                else if (pak == FLAG)
                    state = 1;
                else
                    state = 0;
                break;
        case 4: // Espera Flag - F
                if (pak == FLAG)
                {
                    incoming_frame[i] = pak;
                    i++;
                    STOP = TRUE;
                }
                else
                {
                    incoming_frame[i] = pak;
                    i++;
                }
                break;
        }
    }


    usleep(5);
    STOP = FALSE;

    int successo = 0;

    //Tirar headers dos dados
    char dados_deframed[STUFFED_PACKET_MAXSIZE];  
    char bcc;
    int temp_size = Desfazer_trama(incoming_frame, dados_deframed, s, &bcc);
	if (temp_size < 0 )
		 {successo = -1; printf("desfazer da trama\n");} 
    char dados_destuffed[PACKETMAXSIZE];
    temp_size -= de_stuffing(dados_deframed,dados_destuffed, temp_size);
	
	int j = 0;
	for (j= 0; j<temp_size; j++ )
	{
		if (j == 0)
			BCC_2 = dados_destuffed[j];
		else 
			BCC_2 = (BCC_2 ^ dados_destuffed[j]);
	}

	if ( BCC_2 != bcc)
		{successo = -1; printf("%2x, %2x\n", BCC_2, bcc); }

    if (type == PAK_CMD_FIRST ||type == PAK_CMD_LAST)
    {
        total_number_packets = unpack_control(dados_destuffed, type, filename);
        //printf("N pacotes: %d, Nome Ficheiro: %s\n", total_number_packets, filename);
    }   
    else
    {
        if (unpack_data(dados_obtidos, n_seq, dados_destuffed) != 0)
        {
		printf("No unpacking()\n");
	   successo = -1; 
	}
    }

    //Formular resposta
    char trama_resposta[5];
    int r;
    if (successo == 0)
    {
        if (s == 0)  r = 1; else r = 0;
        fazer_trama_supervisao(trama_resposta, TYPE_RR, EMISSOR, r);
    } else
    {
        fazer_trama_supervisao(trama_resposta, TYPE_REJ, EMISSOR, s);
    }

    //Enviar
    write(fd,trama_resposta,5);

    return successo;
}

int envia_e_espera_dados(char * dados, int s, int size)
{
    //preparar os dados
    char stuffed_data[STUFFED_PACKET_MAXSIZE];
    int temp_size = size + byte_stuffing_encode(dados, stuffed_data, size);
    char framed_data[FRAME_MAXSIZE]; 
    Fazer_trama(temp_size, stuffed_data, s, framed_data, &BCC_2);

    //Formular a resposta esperada
    char res[5]; int r; if (s == 0)  r = 1; else r = 0;
    fazer_trama_supervisao(res, TYPE_RR, EMISSOR, r);

    //Preparar timeout
    memcpy(&Alarm_buffer[0], &framed_data[0], temp_size+6);
    (void) signal(SIGALRM, timeout);
    alarm(2);

    //enviar
    write(fd,Alarm_buffer,temp_size+6);

    //Ficar a espera da resposta
    unsigned char pak;

    int state = 0;
    while(STOP==FALSE)
    {       
        //printf("state: %d\n", state);
        usleep(50);
        read(fd,&pak,1);
        switch (state)
        {
        case 0: //Espera FLAG - F
            if (pak == (char)res[0])
            {
                state++;
            }
            break;
        case 1: //Espera Edreço - A
            if (pak == (char)res[1])
                state++;
            else if (pak == (char)res[0])
                ;
            else
                state = 0;
            break;
        case 2: // Espera Controlo - C
            if (pak == (char)res[2])
                state++;
            else if (pak == (char)res[0])
                state = 1;
            else
                state = 0;
            break;
        case 3: // Espera de BCC
            if (pak == (char)res[3])
                state++;
            else if (pak == (char)res[0])
                state = 1;
            else
                state = 0;
            break;
        case 4: // Espera Flag - F
            if (pak == (char)res[4])
            {
                state = 0;
                STOP = TRUE;
                usleep(50);
                STOP = FALSE;
                return 0;  
                    
            }
            else
                state = 0;
            break;
        }
    }   
    
    return -1;

}
