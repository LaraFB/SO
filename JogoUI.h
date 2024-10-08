#ifndef JogoUI_H
#define JogoUI_H
#include <pthread.h>
#include <ncurses.h> 

#define MAXY 16
#define MAXX 40

#define JOGOENVIARJ "jogoenviarjogos%d"

char JOGADORES_DESTINO[100];
typedef struct janelas janelas,*pjanelas;
typedef struct MSG MSG,*pmsg;

struct MSG{
    char mensagem[500];
    char user[100];
};
struct janelas{
    WINDOW *janelaTopo;
    WINDOW *janelaBaixo;
};
/* typedef thread{

}; */
pthread_t TRecbeMotor;  
pthread_t TEnviaMotor;
pthread_t TComunicacaoJogadores;
pthread_t TTeclado;

//Função para a Validação do Comandos
void ValidacaoComandos();
//Função para desenhar o labirinto
void DesenhaLabirinto(WINDOW *janela, int tipo, char labirinto[MAXY][MAXX]);
//Função que mostra as mensagens do comando
void DesenhaComandos(WINDOW *janela, int tipo);
//Função que recebe teclas
void trataTeclado(WINDOW *janelaTopo, WINDOW *janelaBaixo);
//Função da thread que recebe mensagens do motor
void *RecebeMensagens(void* janelas);
//Função da thread que recebe inputs
void *RecebeTeclado(void* janelas);
#endif