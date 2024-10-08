#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h> 
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <pthread.h>

#include "JogoUI.h"
#include "estruturas.h"
jogo info;

void enviaMensagem(){
    int fdmotor = open(MOTOR,O_RDWR);
    int size = write(fdmotor,&info,sizeof(info));
    //printf("Estou à espera da validacao\n");
    if(size == 0) {
        //wprintw
        printf("[ERRO] ao enviar dados de sessao ao motor!\n");
        //Mandar mensagem ao motor
        exit (-1);
    } 
    close(fdmotor);
}
void EnviaMensagemJogador(char user[100], char mensagem[100],WINDOW *janelaBaixo){
    int i;
    MSG pmensagem;
    for (i=0;i<MAXUSER;i++){
        if(info.Jogadores[i] == NULL){
            wprintw(janelaBaixo,"\n1-Use o comando players");
            return;
            }
        if(strcmp(info.Jogadores[i],user)==0)
            break;
    }
    sprintf(JOGO_DESTINO,JOGO,info.pJogadores[i]);
    int fdjogador = open(JOGO_DESTINO,O_RDWR);
    if( fdjogador == -1){
        wprintw(janelaBaixo,"[ERRO]Erro a abrir o fifo do jogador.\n");
        return;
    } 
    strcpy(pmensagem.mensagem, mensagem);
    strcpy(pmensagem.user,user);
    //Validação do username
    int size = write(fdjogador,&pmensagem,sizeof(pmensagem));
    if(size == 0){
        wprintw(janelaBaixo,"[ERRO] A enviar a mensagem\n");
    }
}

void ValidacaoComandos(WINDOW *janelaBaixo,WINDOW *janelaTopo) {
    char comando[700],aux[100];
    char *user,*cmd,*msg,*msgaux;
    int nsize;

        wprintw(janelaBaixo, "\nIntroduza comando #> ");
        wgetstr(janelaBaixo, comando); //char sem /0
        wprintw(janelaBaixo, "\n [%s] ", comando); 
        strcpy(aux,comando);
        cmd = strtok(aux," ");

        if (strcmp(cmd, "exit") == 0){
            //mandar mensagem ao motor q vai sair para este o fechar tbm
            info.sai = true;
            info.pid = getpid();
            enviaMensagem();

            wprintw(janelaBaixo,"Adeus!\n");
            wclear(janelaTopo); 
            wrefresh(janelaTopo);  
            delwin(janelaTopo); 
            wclear(janelaBaixo);
            wrefresh(janelaBaixo); 
            delwin(janelaBaixo);  
            endwin();  
            exit (0);  
            
        }else if (strcmp(cmd,"msg")==0) {
            char mensagem[100];
            memset(mensagem,'\0',100);
            
            user = strtok(comando," ");
            user = strtok(NULL," ");
            msg = user;
            
            while(msg !=NULL){
                msgaux =strtok(NULL," ");
                if(msgaux==NULL)
                    break;
                msg = strtok(NULL," ");
                if(msg==NULL){
                    sprintf(mensagem, "%s %s", mensagem,msgaux);
                    break;
                }
                sprintf(mensagem, "%s %s %s", mensagem,msgaux,msg);
            }      

            // wprintw(janelaBaixo, "\n[%s]",mensagem);
            if (strcmp(user," ")==0) {
                if(strcmp(mensagem, " ") == 0 || mensagem == NULL) 
                    wprintw(janelaBaixo,"[ERRO] Comando inválido.\n");
            } 
            else
                wprintw(janelaBaixo,"\nEnviado para o utilizador %s ->%s", user,mensagem);
                EnviaMensagemJogador(user,mensagem,janelaBaixo);
                //envia diretamente ao utilizador
               
        }else if(strcmp(cmd,"players")==0){
            wprintw(janelaBaixo,"Lista dos utilizadores.\n"); 
            /* aux.ListaJogadores=true;
            aux.pid=getpid();
            enviaMensagem(); */
            //pede ao motor a lista

        }else
            wprintw(janelaBaixo,"[ERRO] Comando inválido.\n");
   
    wrefresh(janelaBaixo);
}

void DesenhaLabirinto(WINDOW *janela, int tipo, char labirinto[MAXY][MAXX]){
    keypad(janela, TRUE);
    wclear(janela);
    for (int i = 0; i < MAXY; i++) {
        for (int j = 0; j < MAXX; j++) {
            if(labirinto[i][j] != 'p' && labirinto[i][j] != 'b' && labirinto[i][j]!='x')
                wattron(janela, COLOR_PAIR(1)); 
    
            mvwaddch(janela, i, j, labirinto[i][j]);

            if(labirinto[i][j] != 'p' && labirinto[i][j] != 'b' && labirinto[i][j]!='x')
                wattroff(janela, COLOR_PAIR(1));
        }
    }
    refresh(); 
    wrefresh(janela); 
}

void DesenhaComandos(WINDOW *janela, int tipo){
    scrollok(janela, TRUE);                
    refresh(); 
    wrefresh(janela); 
}
void trataTeclado(WINDOW *janelaTopo, WINDOW *janelaBaixo){
    keypad(janelaTopo, TRUE);  
    wmove(janelaTopo, 1, 1);
   
    int tecla = wgetch(janelaTopo);
    int KeyArrows[4];
    
    while (tecla != 113){

        if (tecla == ' '){  // tem o scroll ativo //o wprintw e o wgetstr tem que o scroll ativo.
            echo();
            
            ValidacaoComandos(janelaBaixo,janelaTopo);
                
            noecho(); //voltar a desabilitar o que o utilizador escreve
            wrefresh(janelaBaixo); //sempre que se escreve numa janela, tem de se fazer refresh
        }
        else{          
            for (int i= 0; i< 4; i++)
                KeyArrows[i]=0;  
            if (tecla == KEY_UP){
                KeyArrows[0]=1;
            }  
            else if (tecla == KEY_RIGHT){
                KeyArrows[1]=1;
            }
            else if (tecla == KEY_LEFT){
                KeyArrows[3]= 1;
            }
            else if (tecla == KEY_DOWN){
                KeyArrows[2]=1;
            }
            for (int i= 0; i< 4; i++)
    	        info.movimento[i] = KeyArrows[i];
            info.pid = getpid();
            enviaMensagem();
        }
        wclear(janelaBaixo);
        wmove(janelaTopo, 1, 1); // posiciona o cursor (visualmente) na posicao 1,1 da janelaTopo
        tecla = wgetch(janelaTopo);
    }
}
/* 
void termina(int s, siginfo_t * i, void *v){
    unlink(MOTOR);
    printf("\nAdeus\n");
    exit(1);
    //elimina o seu fifo 
} */

void *RecebeMensagens(void* janelas){
    pjanelas threadjanela = (pjanelas)janelas;
    
    while(1){
        sprintf(LABIRINTO_DESTINO,LABIRINTO,getpid());
        int fd_resposta = open(LABIRINTO_DESTINO, O_RDONLY);
        if(fd_resposta == -1){
            //printf("[ERRO] A enviar mensagens!\n");
            //FECHA MOTOR
            exit(-1);
        }
        int sizeR = read(fd_resposta, &info, sizeof(info));

        if(sizeR > 0){     
            if(strcmp(info.MSG_Motor,"")!= 0)
                wprintw(threadjanela->janelaBaixo,info.MSG_Motor);
            DesenhaLabirinto(threadjanela->janelaTopo, 2, info.labirinto); 
            DesenhaComandos(threadjanela->janelaBaixo, 1); 

            close(fd_resposta);
        }
    }
}       

void *RecebeTeclado(void* janelas){
    pjanelas threadjanela = (pjanelas)janelas;
    trataTeclado( threadjanela->janelaTopo, threadjanela->janelaBaixo); 
}

void *ComunicaoJogadores(void* janelas){
    pjanelas threadjanela = (pjanelas)janelas;
    MSG resposta;
    while(1){
        for(int i=0;i<MAXUSER;i++){
            sprintf(JOGADORES_DESTINO,JOGOENVIARJ,info.pJogadores[i]);
            int fdjogadorr = open(JOGADORES_DESTINO,O_RDWR);
            int sizeR = read(fdjogadorr,&resposta,sizeof(resposta));
            
            if(sizeR > 0){       
                wprintw(threadjanela->janelaBaixo,"%s de %s\n",resposta.mensagem,resposta.user);
                close(fdjogadorr);
            }
        }
    }
}
void MotorFechou(int signo, siginfo_t *info, void *a) {
    printf("O motor vai fechar Adeus.\n");
    sprintf(JOGADORES_DESTINO,JOGOENVIARJ,getpid());
    unlink(JOGADORES_DESTINO);
    sprintf(JOGO_DESTINO,JOGO,getpid());
    unlink(JOGO_DESTINO);
    sprintf(LABIRINTO_DESTINO,LABIRINTO,getpid());
    unlink(LABIRINTO_DESTINO);
    exit(0);
}
int main(int argc, char * argv []){
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = MotorFechou;
    sigaction(SIGUSR1, &sa, NULL);
  //  char labirinto[MAXY][MAXX];
    username pusername;
    username validacao;

    //Verifica nome do utilizador
    if(argc!=2){
        if(argc < 2){
            printf("[ERRO] Falta de argumentos. Introduza o seu nome (./Jogo nome)!\n");
            return -1;
        }
        if(argc > 2){
            printf("[ERRO] Demasiados argumentos.\n");
            return -1;
        }
    }

    strcpy(pusername.user, argv[1]);
    pusername.pid = getpid();

    //Cria o seu named pipe
    sprintf(JOGO_DESTINO, JOGO, getpid());
    mkfifo(JOGO_DESTINO, 0666);

    sprintf(JOGADORES_DESTINO,JOGOENVIARJ, getpid());
    mkfifo(JOGADORES_DESTINO, 0666);

    sprintf(LABIRINTO_DESTINO,LABIRINTO, getpid());
    mkfifo(LABIRINTO_DESTINO, 0666);
    //Verifica se existe Motor  
    int fdmotor = open(MOTOR,O_RDWR);
    if( fdmotor == -1){
        printf("[ERRO] Motor não existe! Compile e corra o ./motor antes.\n");
        return -1;
    } 

    //Validação do username
    int size = write(fdmotor,&pusername,sizeof(pusername));
    printf("Estou à espera da validacao\n");
    if(size == 0) {
        printf("[ERRO] ao enviar dados de sessao ao motor!\n");
        return -1;
    } 
    close(fdmotor);
    
    int fd_resposta = open(JOGO_DESTINO,O_RDONLY);
    int sizeR = read(fd_resposta, &validacao, sizeof(validacao));
    if(sizeR == 0){
        printf("[ERRO] de sessao!\n");
        return -1;
    }
    close(fd_resposta);
    
    if(!validacao.userValid){
        printf("\n[ERRO] Já existe esse nome!\n\n");
        //asistir ao jogo
    } 
    else{      
        //Inicialização do ncurses
        initscr();
        start_color(); 
        
        raw();  
        noecho();
        keypad(stdscr, TRUE); 
        attrset(A_DIM);
        
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
        janelas janela;
        
        janela.janelaTopo = newwin(MAXY, MAXX, 3, 1);
        janela.janelaBaixo = newwin(20, 82, 20, 1);   
        

        wclear(janela.janelaTopo);
        mvprintw(0, 0, "Aguarde pelo fim do tempo de inscriao.");
        mvprintw(1, 0, "O jogo vai começar automaticamente.");
        refresh();
        wrefresh(janela.janelaBaixo); 
        wclear( janela.janelaTopo); 
        
        mvprintw(0, 0, "[Space- muda para o foco da janela de baixo]");
        mvprintw(1, 0, "    [ Bem vindo %s. Boa sorte! ]    ",pusername.user);
        //fazer mais duas janelas, uma para o motor e 
        //outra para flr c os utilizadores 
        /*WINDOW *janelaMotor = newwin();
        WINDOW *janelaJogador = newwin();  */
        if(pthread_create(&TRecbeMotor,NULL,RecebeMensagens, (void *)&janela)!=0){
            wprintw(janela.janelaBaixo,"[ERRO] Ao criar a thread recebe mensagens do motor.\n");
            return -2;
        }
        if(pthread_create(&TTeclado,NULL,RecebeTeclado,(void *)&janela)!=0){
            wprintw(janela.janelaBaixo,"[ERRO] Ao criar a thread trata do teclado.\n");
            return -2;
        }
        if(pthread_create(&TComunicacaoJogadores,NULL,ComunicaoJogadores,(void *)&janela)!=0){
            wprintw(janela.janelaBaixo,"[ERRO] Ao criar a thread recebe mensagens dos jogadores.\n");
            return -2;
        }

        if(pthread_join(TRecbeMotor, NULL)!= 0){
            printf("[ERRO] no fim do tempo de inscrição.\n");
            return -2;
        }
/*         if(pthread_join(TTeclado, NULL)!= 0){
            printf("[ERRO] no fim do tempo de inscrição.\n");
            return -2;
        } */
        wclear(janela.janelaTopo); 
        wrefresh(janela.janelaTopo); 
        delwin(janela.janelaTopo); 
        wclear(janela.janelaBaixo);
        wrefresh(janela.janelaBaixo); 
        delwin(janela.janelaBaixo);  
        endwin(); 
    }
    printf("\nAdeus %s\n",pusername.user);
    return 0;
}

