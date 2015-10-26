#include "filefunc.h"


int fd;
struct termios oldtio,newtio;

volatile int STOP=FALSE; // flag dos alarmes llopen
char Alarm_buffer[FRAME_MAXSIZE];

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

    printf("file_to_buffer(): Terminou com sucesso \n");
    printf("file_to_buffer(): Tamanho do ficheiro: %lu \n", file_size);
    return file_size;
}
int buffer_to_file(char * buffer, char * name, long file_size)
{
    FILE * fp = fopen(name, "a+");

    fwrite(buffer, sizeof(char), file_size, fp);

    fclose(fp);

    return 0;
}
 
int get_chunk(char * res, char * buffer, int chunk_size, int offset, long file_size)
{
    int i = 0;
    for (; i < chunk_size && offset+i < file_size; i++)
    {
            res[i] = buffer[offset+i];

    }

    return i;
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

    strcpy(&res[7], file_name);

    return (7 + sizeof(file_name));
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
    strcpy(file_name, &pak[7]);

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


    char buf[FRAME_MAXSIZE];
    
    if (app == EMISSOR)
    {
        sleep(1);
        int num;
        printf("llread(): A enviar : \n");

        if(fazer_trama_supervisao(buf,TYPE_SET,EMISSOR, 0)==-1)
            return 1;
        num =write(fd,buf,5);
        printf(" numero de bytes %d \n",num);
        printf("%2x, %2x, %2x, %2x, %2x\n",buf[0],buf[1],buf[2],buf[3],buf[4]);

        return 0;
        
    } else if (app == RECETOR)
    {
    printf("llread(): A enviar : \n");
    STOP=FALSE;
    
    unsigned char pak;
    int state = 0;
        while (STOP==FALSE) 
        {
            usleep(50);
            read(fd,&pak,1);
            printf("%2x : \n",pak);
            switch (state)
            {
                case 0: //Espera FLAG - F
                    if (pak == FLAG)
                    {   
                        buf[0]=FLAG;
                       state++;
                    }
                break;
                case 1: //Espera AE ou AR
                    if (pak == AE)
                    {
                       buf[1]=AE;
                       state++;
                    }else if(pak==AR){
                       buf[1]=AR;
                       state++;
                    }
                break;
                case 2: //Espera CSET CDISC CUA CRR(r_num) CREJ(r_num)
                    if (pak == CSET)
                    {
                       buf[2]=CSET;
                       state++;
                    }else if(pak==CDISC){
                       buf[2]=CDISC;
                       state++;
                    }//continuar mais tarde só para motivos de testes
                break;
                case 3: //Espera xor entre buf[1])^buf[2]
                    if (pak == (char)(buf[1]^buf[2]) )
                    {
                       buf[3]=(char)(buf[1])^buf[2];
                       state++;
                    }
                break;
                case 4: //Espera AE ou AR
                    if (pak == FLAG)
                    {
                       buf[4]=FLAG;
                       state++;
                    }
                break;
                case 5: //Espera AE ou AR
                    STOP=TRUE;
                break;
            }
        }
    printf("%2x, %2x, %2x, %2x, %2x: \n",buf[0],buf[1],buf[2],buf[3],buf[4]);
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
		alarm(2);
	}
}


int espera_e_responde_superv(char * msg, char * res)
{

    //RECEBER SET
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

    // Enviar UA resposta
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
				sleep(2);
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

int envia_e_espera_dados(char * msg, char * res)
{
	
    return 0;
}