#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H
#include <stdbool.h>

#define MAXY 16
#define MAXX 40
#define MAXUSER 5

#define MOTOR "motorfifo"
#define JOGO "jogofifo%d"
#define LABIRINTO "labirintofifo%d"

char JOGO_DESTINO[100];
char LABIRINTO_DESTINO[100];
typedef struct jogo jogo, *pjogo;

typedef struct{
    pid_t pid;
    char user[100];
    int userValid;
}username;

struct jogo{
    //motor stuff
    int nivel; 
    int pedras;
    int barreira;
    bool alteracao;
    pid_t pJogadores[MAXUSER];
    pthread_mutex_t mutex;
    char labirinto[MAXY][MAXX];
    char MSG_Motor[1000]; 
    char *Jogadores[MAXUSER];
    //jogador stuff
    pid_t pid;
    int movimento[3]; //manda array com 4 espaços 0000, 1000- cima, 0100- direita, 0010- baixo, 0001-esquerda 
    bool sai;
    bool ListaJogadores; //pede o players true ou false
};

#endif