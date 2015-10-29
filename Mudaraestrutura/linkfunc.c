#include "linkfunc.h"


struct linkLayer Linkdata;

volatile int STOP=FALSE;


int llopen(int port, int user){

    //Mudar isto depois
    Linkdata.user = user;
    Linkdata.portfd = port;
    Linkdata.ALTERNATING = 0;

    //Construir Trama SET e UA
    if(fazer_trama_supervisao(Linkdata.frame_envio, TYPE_SET, EMISSOR, 0) == -1)
        return -1;
    if (fazer_trama_resposta(Linkdata.frame_resposta, Linkdata.frame_envio) == -1)
        return -1;

    if (user == EMISSOR)
    {
        if (envia_e_espera_superv(port, Linkdata.frame_envio, Linkdata.frame_resposta) != -1)
        return Linkdata.portfd;
    } 
    else if(user == RECETOR)
    {
        if (espera_e_responde_superv(port, Linkdata.frame_envio, Linkdata.frame_resposta) != -1)
        return Linkdata.portfd;
    }

    return -1;
}
int llclose(int port_fd)
{

    //Construir Trama SET
    if(fazer_trama_supervisao(Linkdata.frame_envio, TYPE_DISC, EMISSOR, 0) == -1)
        return -1;
    if (fazer_trama_resposta(Linkdata.frame_resposta, Linkdata.frame_envio) == -1)
        return -1;

    
    int final = 0;
    if(Linkdata.user == EMISSOR)
    {
        final = envia_e_espera_superv(port_fd, Linkdata.frame_envio, Linkdata.frame_resposta);
        if (final != -1)
        {
            fazer_trama_supervisao(Linkdata.frame_envio, TYPE_UA, EMISSOR, 0);
            write(port_fd,Linkdata.frame_envio,5);
        }
        return final;
    }
    else if(Linkdata.user == RECETOR)
    {
        final = espera_e_responde_superv(port_fd, Linkdata.frame_envio, Linkdata.frame_resposta);
        if (final != -1)
        {
            fazer_trama_supervisao(Linkdata.frame_envio, TYPE_UA, EMISSOR, 0);
            int i; final = 0; char pak;
            for (i = 0; i < 5; i++)
            {   
                read(port_fd,&pak,1);
                if (pak != (char)Linkdata.frame_envio[i])
                    final = -1;
            }
        }
        
    }
    

    return -1;   
}

int fazer_trama_supervisao(char * res, int type, int direction, int r_num){
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
int fazer_trama_resposta(char * res, char * msg){
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
int espera_e_responde_superv(int port, char * msg, char * res)
{

    unsigned char pak;
    int state = 0;
    while (STOP==FALSE) 
    {
        usleep(50);
        read(port,&pak,1);

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
    write(port,res,5);    
    STOP = FALSE;
    return 0;
}
int envia_e_espera_superv(int port, char * msg, char * res)
{
    //Preparar a função de timeout
    (void) signal(SIGALRM, timeout);
    Linkdata.timeout = 2;
    alarm(2);

    //enviar
    write(port,Linkdata.frame_envio,5);
    
    //Ciclo de Espera
    unsigned char pak;
    
    
    int state = 0;
    while(STOP==FALSE)
    {       
        usleep(50);
        read(port,&pak,1);
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


void timeout()
{
    if (STOP == FALSE)
    {
        printf("Ocorreu time out\n");
        write(Linkdata.portfd,Linkdata.frame_envio,5);
        alarm(Linkdata.timeout);
    }
}



int llread(int port_fd, char * message)
{
    Linkdata.portfd = port_fd;
    int tamanho_frame = espera_dados();     //Linkdata.frame_resposta


    //Tirar headers dos dados
    char dados_deframed[STUFFED_PACKET_MAXSIZE]; 
    char BCC2; 
    int tamanho_deframed = Desfazer_trama(Linkdata.frame_resposta, dados_deframed, Linkdata.ALTERNATING, &BCC2);


    //Tirar stuffing aos dados
    char dados_destuffed[PACKETMAXSIZE];
    int tamanho_destuffed = tamanho_deframed - de_stuffing(dados_deframed,dados_destuffed, tamanho_deframed);

    //E preciso verificar BCC2
    // ! // ! // ! // ! // ! // ! // ! // ! // ! // ! //
    // ! // ! // ! // ! // ! // ! // ! // ! // ! // ! //
    // ! // ! // ! // ! // ! // ! // ! // ! // ! // ! //

    //copiar para buffer
    memcpy(message, dados_destuffed, tamanho_destuffed);

    return 0;
}

int espera_dados()
{

    unsigned char pak;
    int i = 0;
    int state = 0;

    while (STOP==FALSE) 
    {
        //printf("state: %d\n", state);
        usleep(5);
        read(Linkdata.portfd,&pak,1);
        switch (state)
        {
        case 0: //Espera FLAG - F
                if (pak == FLAG)
                {   
                    Linkdata.frame_resposta[i] = pak;
                    i++;
                    state++;
                }
                break;
        case 1: //Espera Edreço - A
                if (pak == AE)
                {
                    Linkdata.frame_resposta[i] = pak;
                    i++;
                    state++;
                }       
                else if (pak == FLAG)
                    i = 1;
                else
                    state = 0;
                break;
        case 2: // Espera Controlo - C
                if (pak == CDATA(Linkdata.ALTERNATING))
                {

                    Linkdata.frame_resposta[i] = pak;
                    i++;
                    state++;
                }
                else if (pak == FLAG)
                        state = 1;
                else
                {
                    printf("1- %2x, 2- %2x \n", pak ,(AE ^ CDATA(0)));
                    state = 0;
                }    
                break;
        case 3: // Espera de BCC
                if (pak == (AE ^ CDATA(Linkdata.ALTERNATING)) )
                {
                    Linkdata.frame_resposta[i] = pak;
                    i++;
                    state++;
                }
                else if (pak == FLAG)
                    state = 1;
                else
                {
                    state = 0;
                }    
                break;
        case 4: // Espera Flag - F
                if (pak == FLAG)
                {
                    Linkdata.frame_resposta[i] = pak;
                    i++;
                    STOP = TRUE;
                }
                else
                {
                    Linkdata.frame_resposta[i] = pak;
                    i++;
                }
                break;
        }
    }

    usleep(5);
    STOP = FALSE;

    return i;
}
void enviar_RR_REJ(int successo)
{
    char trama_resposta[5];
    if (successo == 0)
    {
        if (Linkdata.ALTERNATING == 0) Linkdata.ALTERNATING = 1; else Linkdata.ALTERNATING = 0;
        fazer_trama_supervisao(trama_resposta, TYPE_RR, EMISSOR, Linkdata.ALTERNATING);
    } 
    else
    {
        fazer_trama_supervisao(trama_resposta, TYPE_REJ, EMISSOR, Linkdata.ALTERNATING);
    }

    write(Linkdata.portfd,trama_resposta,5);
}

int llwrite(int port_fd, char * message, int length)
{
    Linkdata.portfd = port_fd;

    //preparar os dados
    char stuffed_data[STUFFED_PACKET_MAXSIZE];
    int temp_size = 
        length + byte_stuffing_encode(message, stuffed_data, length);
    char bcc = (AE^CDATA(Linkdata.ALTERNATING)); //completar bcc
    temp_size = Fazer_trama(temp_size, stuffed_data, Linkdata.frame_envio, &bcc);

    //Formular a resposta esperada
    int r; if (Linkdata.ALTERNATING == 0)  r = 1; else r = 0;
    fazer_trama_supervisao(Linkdata.frame_resposta, TYPE_RR, EMISSOR, r);

    //Envio da trama e recepçao de respostas
    envia_e_espera_dados(temp_size);

    if (Linkdata.ALTERNATING == 0) Linkdata.ALTERNATING = 1; else Linkdata.ALTERNATING = 0;

    return 0;
}
int envia_e_espera_dados(int size)
{
    
    //Preparar timeout
    (void) signal(SIGALRM, timeout);
    alarm(2);

    //enviar
    write(Linkdata.portfd,Linkdata.frame_envio,size);

    //Ficar a espera da resposta
    unsigned char pak;

    int state = 0;
    while(STOP==FALSE)
    {       
        //printf("state: %d\n", state);
        usleep(50);
        read(Linkdata.portfd,&pak,1);
        switch (state)
        {
        case 0: //Espera FLAG - F
            if (pak == (char)Linkdata.frame_resposta[0])
            {
                state++;
            }
            break;
        case 1: //Espera Edreço - A
            if (pak == (char)Linkdata.frame_resposta[1])
                state++;
            else if (pak == (char)Linkdata.frame_resposta[0])
                ;
            else
                state = 0;
            break;
        case 2: // Espera Controlo - C
            if (pak == (char)Linkdata.frame_resposta[2])
                state++;
            else if (pak == (char)Linkdata.frame_resposta[0])
                state = 1;
            else
                state = 0;
            break;
        case 3: // Espera de BCC
            if (pak == (char)Linkdata.frame_resposta[3])
                state++;
            else if (pak == (char)Linkdata.frame_resposta[0])
                state = 1;
            else
                state = 0;
            break;
        case 4: // Espera Flag - F
            if (pak == (char)Linkdata.frame_resposta[4])
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

int Fazer_trama(int tamanho_dados, char * dados, char * res, char * bcc2){

    if(tamanho_dados> STUFFED_PACKET_MAXSIZE)
        return -1;  

    res[0] = FLAG;
    res[1] = AE;
    res[2] = CDATA(Linkdata.ALTERNATING);
    res[3] = (AE ^ CDATA(Linkdata.ALTERNATING));

    memcpy(&res[4], &dados[0], tamanho_dados);
    memcpy(&res[4+tamanho_dados],&bcc2, 1);
    res[5+tamanho_dados] =  FLAG;
    
    return tamanho_dados+6;
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