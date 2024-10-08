#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <ctype.h>

#include "estruturas.h"
#include "Motor.h"
jogo aux;

//PROBLEMAS:
//QUANDO MAIS QUE UM JOGADOR SE O JOGO ANTIGO CONECTA AQUILO MORRE
//Aceita jogadores para smp
va variaveis;
void VariavelAmbiente(){ 
    if(getenv("INSCRICAO") == NULL) {
        setenv("INSCRICAO", INSCRICAO, 1);
        variaveis.inscricao = atoi(INSCRICAO);
    }
    variaveis.inscricao  = (int) strtol(getenv("INSCRICAO"), NULL, 10);
    
    if(getenv("NPLAYERS") == NULL) {
        setenv("NPLAYERS", NPLAYERS, 1);
        variaveis.nplayers= atoi(NPLAYERS);
    }
    variaveis.nplayers= strtol(getenv("NPLAYERS"), NULL, 10);
    if(getenv("DURACAO") == NULL) {
        setenv("DURACAO", DURACAO, 1);
        variaveis.duracao= atoi(DURACAO);
    }
    variaveis.duracao =  strtol(getenv("DURACAO"), NULL, 10);
    if(getenv("DECREMENTO") == NULL) {
        setenv("DECREMENTO", DECREMENTO, 1);
        variaveis.decremento = atoi(DECREMENTO);
    }
    variaveis.decremento = strtol(getenv("DECREMENTO"), NULL, 10);
    printf("Duração do primeiro nível: %d.\n", variaveis.duracao);
    printf("Tempo de Inscrição: %d.\n",variaveis.inscricao);
    printf("Tempo para o primeiro nível: %d.\n", variaveis.duracao);
    printf("Próximo nível tem menos este tempo: %d.\n",variaveis.decremento);   
}

void CloseJogadores(){
    union sigval val;
	val.sival_int = 2;

    for(int i=0; i<MAXUSER;i++)
        if(listaUsers[i]==1)
            sigqueue(pidJogadores[i], SIGUSR1, val);
}

//Fecha tudo
void CloseMotor(){
    printf("Motor a encerrar!\n");
    
    CloseJogadores();
    
    close(fdmotor);
    unlink(MOTOR);
    printf("Motor encerrou...\n");
    exit(0);     
}

//Valida Jogadores 
bool ValidaoUser(char *novoJogador){
    if(strcmp(novoJogador, " ") == 0)
        return false;
        
    for(int i= 0; i< MAXUSER; i++)
        //printf("\n%d, Nomes existentes: \n%s",i, nomeUsers[i]);
        if(strcmp(nomeUsers[i], novoJogador) == 0)
            return false;
            
    return true;
}

//Ler ficheiro 
void LeFicheiroLabirinto(char arraylabirinto[MAXY][MAXX], const char *nomeficheiro) {
    FILE *f = fopen(nomeficheiro, "r");

    if (f == NULL) {
        perror("[ERROR] A abrir o ficheiro.\n");
        exit(2);
    }

    for (int i = 0; i < MAXY; i++) {
        if (fgets(arraylabirinto[i], MAXX + 2, f) == NULL) {
            printf("[ERROR] A ler o ficheiro.\n");
            exit(2);
        }
    }
    fclose(f);
}

//Inicializa com coordenadas random
int CoordenadasIniciais(int n){
    srand(time(NULL));
    int coordenada = rand() % n; 
    
    return coordenada;
}

//Valida coordenadas
bool ValidaCordenadas(int x, int y, char labirinto[MAXY][MAXX]){
    if(labirinto[y][x] == ' ')
        return true;
    return false;
}
/* 
int MostraBot(){    
    int aux[2];
    int nbytes;
    char str[100];
    int SegPedra [] = {30,25,20,15};

    printf("[Motor] Inicializando o Bot.\n");
    pipe(aux);

    int res = fork();
    if(res==0){
        printf("[Motor] (PID=%d)\n",getpid());
        close(aux[0]);
        close(1);
        dup(aux[1]);
        close(aux[1]);
        execl("bot","bot","4","10",NULL);
        printf("[Motor] Não consegui executar o bot\n");
        exit(1);    
    } 
    close(aux[1]);

    int nvezes = 0;
    do{
        printf("%d",nvezes);
        nvezes++;
        nbytes = read(aux[0],&str,99);
        if(nbytes>0){
            //str[nbytes]='\0';
            printf("[Motor] Valores do Bot: %s\n",str);
        }  
    }while(nvezes<10);//);

    union sigval val;
    sigqueue(res,SIGINT,val); 
    
    wait(&res); 
    close(aux[0]);  
    return 1;
}
 void termina_bot(int s, siginfo_t *i, void *v){
    for (int i = 0 ; i <100 ; i ++)
         kill(listaUsers[i],SIGINT);
    unlink(MOTOR);
    exit(1);
} */
void KickJogador(char *nome){
    for (int i=0; i<MAXUSER;i++){
        if (strcmp(nomeUsers[i], nome) == 0) {
            union sigval val;
            val.sival_int = 2;
            sigqueue(pidJogadores[i], SIGUSR1, val);
            break;
        }
    }
    printf("Jogador %s não encontrado.\n", nome);
}

/* void RemoveBloqueioMovel() {
    pthread_cancel(TMovel);
    pthread_join(TMovel, NULL);
}
void * MoverBloqueioMovel(){
    while(1){
        if(x<MAXX){
            x++;
        if(ValidaCordenadas(x,y,aux.labirinto))
            aux.labirinto[y][x]='b';
            aux.labirinto[y][--x]=' ';
        }else{
            x++;
        if(ValidaCordenadas(x,y,aux.labirinto))
            aux.labirinto[y][x]='b';
            aux.labirinto[--y][x]=' ';}
    }

} */

void ValidacaoComandos() {
    char comando[100], caux [100];
    char *cmd;

    while (1) {
        fgets(comando, sizeof(comando), stdin); 
        strcpy(caux,comando);
        cmd = strtok(caux," ");

        if (strcmp(cmd,"users\n")==0){  
            printf("Lista de utilizadores:\n");
            for (int i = 0; i< MAXUSER; i++)
                printf("- %s.\n",nomeUsers[i]);

            //ListaJogadores(lista);
        }else if (strcmp(cmd,"kick")==0) {
            int arg = 0, i = 0;
           
            while (comando[i] != '\n') { 
                if (comando[i]==' ')
                    arg++; 
                i++; 
            } 
            if(arg==1){
                char *user;
                user = strtok(comando, " ");
                user = strtok(NULL, " ");
               // strcat(user,"\0");
                KickJogador(user);
            }
            else
                printf("[ERRO] Comando inválido.\n");
             

        }else if (strcmp(cmd, "bots\n") == 0) {
           printf("Lista de bots. \n"); 

        }else if (strcmp(cmd, "bmov\n") == 0) {
            printf("Bloqueio móvel introduzido.\n");
            int x,y;   
            while(1){
                x=CoordenadasIniciais(MAXX);
                y=CoordenadasIniciais(MAXY);
                if(ValidaCordenadas(x,y,aux.labirinto)){
                    aux.labirinto[y][x]='b';
                    aux.alteracao = true;
                  //  pthread_create(&TMovel, NULL, MoverBloqueioMovel, NULL);
                    break;
                }
            }
            
        }else if (strcmp(cmd, "rbm\n") == 0) {
            printf("Bloqueio móvel removido.\n");
            //RemoveBloqueioMovel();
            //elimina a mais antiga

        }else if (strcmp(cmd, "begin\n") == 0) {
            printf("Jogo iniciado.\n");
            incricaoEncerrada = 1;

        }/* else if (strcmp(cmd, "test_bot\n") == 0) {
            printf("Bot a funcionar:\n");
            MostraBot();
            
        } */else if(strcmp(cmd, "end\n") == 0){  
            printf("\nAdeus administrador!\n\n");
            CloseMotor();
            exit (0);
        }else
            printf("[ERRO] Comando inválido.\n");
    }
}

void DesenhaLabirinto(char labirinto[MAXY][MAXX]){
    for (int i = 0; i < MAXY; i++) {
        for (int j = 0; j < MAXX; j++) {
            printf("%c", labirinto[i][j]);
        }
       //printf("\n");
    }
}

void ProxNivel(){
    aux.nivel++;
    if(aux.nivel == 4){
        printf("Fim do jogo\n");
        CloseMotor();
    }
    sprintf(variaveis.ficheiro,"labirinto%d.txt",aux.nivel);
    printf("%s\n",variaveis.ficheiro);
    LeFicheiroLabirinto(aux.labirinto,variaveis.ficheiro);
    int x;
    for(int i=0;i<MAXUSER;i++){
        if(listaUsers[i] == 1){
            while(1){
                x = CoordenadasIniciais(39);
                if(ValidaCordenadas(x,1,aux.labirinto));
                    break;  
            }
            aux.labirinto[1][x]= toupper(nomeUsers[i][0]);
        }
    }
    variaveis.duracao = variaveis.duracao - variaveis.decremento;
    segundosrestantes = variaveis.duracao;
    
    //recomeçar o tempo
    alarm(1);
   
}
void TempoNivel(int sinal){
    if(sinal == SIGALRM){
        printf("Segundos restantes: %d\n", segundosrestantes--);
        alarm(1);
    }
    if(segundosrestantes == 0){
        strcpy(aux.MSG_Motor, "O tempo acabou");
        ProxNivel();
    }
}

void *ContaTempoD(){
    while (1) {
        pthread_mutex_lock(&mutexTempoInscricao);
        if (variaveis.inscricao <= 0) {
            pthread_mutex_unlock(&mutexTempoInscricao);
            incricaoEncerrada = 1;
            break;  
        }
        if(incricaoEncerrada)
            break;
        else{
        printf("\n%ds", variaveis.inscricao);
        pthread_mutex_unlock(&mutexTempoInscricao);

        sleep(1);
        pthread_mutex_lock(&mutexTempoInscricao);
        variaveis.inscricao--;
        pthread_mutex_unlock(&mutexTempoInscricao);
        }
    }
    pthread_exit(NULL);
} 

void *RecebeJogador(){
    fdmotor = open(MOTOR,O_RDONLY);   
    if(fdmotor == -1)
       exit(-2);

    username userRecebido;
    username userEnviado;
    int x;

    while(1){
        if(incricaoEncerrada){
            printf("Acabou o tempo de inscrição.\n");
            break;
        } 
        int sizeR = read(fdmotor, &userRecebido, sizeof(userRecebido));
        
        if (sizeR > 0){
            printf("\nRecebi jogador: %s\n",userRecebido.user);
            userEnviado.pid = userRecebido.pid;

            //Valida Users
            if(ValidaoUser(userRecebido.user)){

                // Verifica se há espaço para receber
                for (int i = 0; i<MAXUSER; i++){
                    if(listaUsers[i] == 0){
                        nomeUsers[i]= malloc(strlen(userRecebido.user) + 1);
                        strcpy(nomeUsers[i], userRecebido.user);
                        userEnviado.userValid= true;
                        listaUsers[i]= 1;
                        pidJogadores[i]= userRecebido.pid; 
                        
                         while(1){
                            x = CoordenadasIniciais(39);
                            if(ValidaCordenadas(x,1,aux.labirinto));
                                break;  
                        }
                        aux.labirinto[1][x]= toupper(nomeUsers[i][0]);
                        letras[i] =  toupper(nomeUsers[i][0]);
                        //Não aceitar a mesma letra
                        break;
                    }
                }
            }
            else
                userEnviado.userValid = false;
                //fica a ver o jogo
        }  
        sprintf(JOGO_DESTINO,JOGO,userRecebido.pid);
        int fd_resposta = open (JOGO_DESTINO,O_WRONLY|O_NONBLOCK);
        int enviado= write(fd_resposta,&userEnviado,sizeof(userEnviado));
        close (fd_resposta);
    }
    pthread_exit(NULL);
}

void *Comandos(){
    ValidacaoComandos();
    pthread_exit(NULL);
}

void *EnviaJogador(){
    while(1){
        if(aux.alteracao){
            for(int i= 0; i<MAXUSER; i++){
                if(listaUsers[i]==1){
                    sprintf(LABIRINTO_DESTINO,LABIRINTO,pidJogadores[i]);
                    int fd_envia_jogo = open(LABIRINTO_DESTINO, O_WRONLY);
                    if(fd_envia_jogo == -1){
                        printf("[ERRO] A enviar mensagens!\n");
                        CloseMotor();
                        exit(-1);
                    }
                    int enviado= write(fd_envia_jogo,&aux,sizeof(aux));
                    close (fd_envia_jogo);  
                }     
            }
            aux.alteracao=false; 
        }
    }
}

void *RecebePedido(){
    int i, j, x, y;
    while(1){
        int fdmotor = open(MOTOR,O_RDONLY);   
        int sizeR = read(fdmotor, &aux, sizeof(aux));
        if (sizeR > 0){
            if(aux.sai){
                char msgaux[100];
                for (int i= 0; i<MAXUSER; i++){
                    if(pidJogadores[i] == aux.pid){
                        printf("[MOTOR]Saiu o user %s\n",nomeUsers[i]);
                        sprintf(msgaux, "Saiu o user %s",nomeUsers[i]);
                        strcpy(aux.MSG_Motor,msgaux);
                        listaUsers[i] = 0;
                        pidJogadores[i] = 0;
                        strcpy(nomeUsers[i]," ");
                        aux.alteracao= true;
                    }
                }
                strcpy(aux.MSG_Motor, " ");
            }
            
            for(i=0;i<4;i++)
                if(aux.movimento[i] == 1)
                    break;
            
            for(j=0; j<MAXUSER;j++)    
                if(aux.pid == pidJogadores[j])
                    break; 

            bool acaba_ciclo = false;         
            for (y=0; y<MAXY; y++){
                for(x=0; x<MAXX; x++){
                    if(aux.labirinto[y][x] == letras[j]){
                        acaba_ciclo = true;
                        break;
                    }
                }
                if(acaba_ciclo)
                    break;
            }  
            if(y == 15){
                char saux[1000];
                sprintf(saux,"O jogador %c concluiu o nivel!Proximo Nivel...", letras[j]);
                strcpy(aux.MSG_Motor,saux);
                ProxNivel();
            }
            switch (i){
                case 0:
                    y--;
                    if(ValidaCordenadas(x,y,aux.labirinto)){
                        aux.labirinto[y][x] = letras[j];
                        aux.labirinto[++y][x] = ' ';
                    }
                break;
                case 1:
                    x++;
                    if(ValidaCordenadas(x,y,aux.labirinto)){
                        aux.labirinto[y][x] = letras[j];
                        aux.labirinto[y][--x] = ' ';
                    }
                break;
                case 2:
                    y++;
                    if(ValidaCordenadas(x,y,aux.labirinto)){
                        aux.labirinto[y][x] = letras[j];
                        aux.labirinto[--y][x] = ' ';
                    }
                break;
                case 3:
                    x--;
                    if(ValidaCordenadas(x,y,aux.labirinto)){
                        aux.labirinto[y][x] = letras[j];
                        aux.labirinto[y][++x] = ' ';
                    }
                break;   
            }
            aux.alteracao=true;
            //DesenhaLabirinto(aux.labirinto);
        } 
    }
}

void TempoPermanenciaAcabou(int sinal) {
    printf("[BOT] O tempo de permanência da pedra expirou...\n");
    //aux.labirinto[y][x]=' ';
    //aux.alteracao= true;
}

void TempoPermanencia(int segundos){
    signal(SIGALRM, TempoPermanenciaAcabou); 
    alarm(segundos);
}

void *LancaBot(){
    while(1){
        int numBots;
        int intervalos[] = {30, 25, 20, 15};
        int duracoes[][4] = {
            {10, 5, 0, 0},
            {15, 10, 5, 0},
            {20, 15, 10, 5}
        };

        switch (aux.nivel) {
            case 1:
                numBots = 2;
                break;
            case 2:
                numBots = 3;
                break;
            case 3:
                numBots = 4;
                break;
        }

        printf("[BOT] Nível %d.\n", aux.nivel);
        
        for (int i = 0; i < numBots; i++) {
            int paux[2];
            pipe(paux);

            int res = fork();
            if (res == 0) {
                printf("[MOTOR] (Bot %d - PID=%d)\n", i + 1, getpid());
                close(paux[0]);
                close(1);
                dup(paux[1]);
                close(paux[1]);
                char str[2];
                sprintf(str, "%d", duracoes[aux.nivel - 1][i]);
                execl("bot", "bot", "4", str, NULL);
                printf("[MOTOR] Não consegui executar o bot\n");
                exit(1);
            }
            close(paux[1]);

            int num[3];
            char str[100];
            int nbytes = read(paux[0], &str, sizeof(str) - 1);
            if (nbytes > 0) {
                str[nbytes] = '\0';
                
                sscanf(str, "%d %d %d", &num[0], &num[1], &num[2]);
                printf("[Motor] Valores do Bot %d: %d %d %d\n", i + 1, num[0], num[1], num[2]);
                aux.labirinto[num[1]][num[0]]='p';
                aux.alteracao=true;
                TempoPermanencia(num[2]);
               // MataBot(res);
            }
            close(paux[0]);
        }
    }
}

int main(){
    VariavelAmbiente();     
    aux.alteracao = false;
    aux.nivel = 1;
/*     struct sigaction sa;
    sa.sa_sigaction = termina_bot;
    sigaction(SIGINT, &sa, NULL) */; 

    //verifica se existe mais motores
    if(mkfifo(MOTOR,0666) == -1){
        if(errno == EEXIST)
            printf("[ERRO] Motor ja existe!\n");
        return -1;
    }
    if(mkfifo(LABIRINTO,0666) == -1){
        if(errno == EEXIST)
            printf("[ERRO] O fifo do Labirinto ja existe!\n");
        return -1;
    }

    printf("Bem vindo administrador!\n");
    sprintf(variaveis.ficheiro,"labirinto%d.txt",aux.nivel);
    printf("%s\n",variaveis.ficheiro);
    LeFicheiroLabirinto(aux.labirinto,variaveis.ficheiro);
    //DesenhaLabirinto(aux.labirinto);
    
    signal(SIGINT,CloseMotor);
    if(pthread_create(&TTempoInscricao, NULL, ContaTempoD, NULL) != 0) {
        printf("[ERRO] Ao criar a thread tempo de inscricao.\n");
        CloseMotor();
        return -2;
    }
    if(pthread_create(&TRecebeJogador,NULL,RecebeJogador,NULL)!=0){
        printf("[ERRO] Ao criar a thread inscricao.\n");
        CloseMotor();
        return -2;
    }
    if(pthread_create(&TComandos,NULL,Comandos,NULL)!=0){
        printf("[ERRO] Ao criar a thread inscricao.\n");
        CloseMotor();
        return -2;
    }
   
    if(pthread_join(TTempoInscricao, NULL)!= 0){
        printf("[ERRO] no fim do tempo de inscrição.\n");
        CloseMotor();
        return -2;
    }
    if(pthread_join(TRecebeJogador, NULL)!= 0){
        printf("[ERRO] no fim do tempo de inscrição.\n");
        CloseMotor();
        return -2;
    }
    //Acabou a inscricao
    aux.alteracao=true;
    if (signal(SIGALRM, TempoNivel) == SIG_ERR) {
        printf("Erro ao configurar manipulador de sinal");
        CloseMotor();
    }
    segundosrestantes = variaveis.duracao;
    alarm(1);

    if(pthread_create(&TEnviaInfo,NULL,EnviaJogador,NULL)!=0){
        printf("[ERRO] Ao criar a thread para enviar informação aos jogadores.\n");
        CloseMotor();
        return -2;
    }
    if(pthread_create(&TRecbeInfo,NULL,RecebePedido,NULL)!=0){
        printf("[ERRO] Ao criar a thread para recebe informação aos jogadores.\n");
        CloseMotor();
        return -2;
    }
    if(pthread_create(&TBots,NULL,LancaBot,NULL)!=0){
        printf("[ERRO] Ao criar a thread para lança os bots.\n");
        CloseMotor();
        return -2;
    }
    if(pthread_join(TEnviaInfo, NULL)!= 0){
        printf("[ERRO] no fim do tempo de inscrição.\n");
        CloseMotor();
        return -2;
    }
    CloseMotor();
    return 0;
}