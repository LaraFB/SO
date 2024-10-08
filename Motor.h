#ifndef Motor_H
#define Motor_H
#include <pthread.h>

#define MAXUSER 5 
#define MAXBOT 10
#define MAXPEDRA 50
#define MAXNIVEL 3
#define MAXBMOVEL 5
#define MAXY 16
#define MAXX 40

#define NFICHEIRO1 "labirinto1.txt"
#define NFICHEIRO2 "labirinto2.txt"
#define NFICHEIRO3 "labirinto3.txt"

#define INSCRICAO "20"
#define NPLAYERS "1"
#define DURACAO "70"
#define DECREMENTO "20"

char *nomeUsers[MAXUSER] = {"","","","",""};
char letras[MAXUSER];
pid_t pidJogadores[MAXUSER];
int listaUsers[MAXUSER]={0};

int fdmotor;
int incricaoEncerrada = 0;
int segundosrestantes;

pthread_t TTempoInscricao; 
pthread_t TMovel; 
pthread_t TRecebeJogador;
pthread_t TComandos;
pthread_t TRecbeInfo;
pthread_t TBots;
pthread_t TEnviaInfo;
pthread_mutex_t mutexInfo = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexTempoInscricao =  PTHREAD_MUTEX_INITIALIZER;

typedef struct pedra pedra,*ppedras;
typedef struct barreira barreira,pbarreira;
typedef struct va va, *pva;

struct va{
    int inscricao;
    int decremento;
    int nplayers;
    int duracao;
    char ficheiro[100];
};

/* typedef struct Jogador{
    char nome[50];
    int px;
    int py;
    int pontuacao;
    struct Jogador *prox;
}jogador_t;
 */
/* struct Pedra{
    int px;
    int py;
    int numpedras;  
};

struct Barreira{
    int px;
    int py;
    int numbarreiras;
}; */

// Funções do Motor

//Varíaveis ambiente
void VariavelAmbiente();
void DesenhaLabirinto(char labirinto[MAXY][MAXX]);
void ValidacaoComandos();
int MostraBot();
void LeFicheiroLabirinto(char arraylabirinto[MAXY][MAXX], const char *nomeficheiro);

#endif 