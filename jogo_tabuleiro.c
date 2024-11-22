#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Atividade desenvolvida por Diego Contini e Andre Leoncio

/// Cria as structs necessárias para o jogo
typedef struct
{
    int numero;
    int especial;
    char acao[100];
} Casa;

typedef struct Carta
{
    char descricao[100];
    int acao;
} Carta;

typedef struct NoPilhaCarta
{
    Carta carta;
    struct NoPilhaCarta *proximo;
} NoPilhaCarta;

typedef struct Pilha
{
    NoPilhaCarta *topo;
} Pilha;

typedef struct Jogador
{
    char nome[50];
    int posicao;
    int pular_turno;
} Jogador;

typedef struct NoFila
{
    Jogador jogador;
    struct NoFila *proximo;
} NoFila;

typedef struct Fila
{
    NoFila *frente;
    NoFila *tras;
} Fila;

// Organiza as cartas em uma pilha
void empilharCarta(Pilha *pilha, Carta carta)
{
    NoPilhaCarta *novoNo = (NoPilhaCarta *)malloc(sizeof(NoPilhaCarta));
    novoNo->carta = carta;
    novoNo->proximo = pilha->topo;
    pilha->topo = novoNo;
}

// Desempilha a carta do topo
Carta desempilharCarta(Pilha *pilha)
{
    if (pilha->topo == NULL)
    {
        printf("Pilha vazia!\n");
        exit(1);
    }
    NoPilhaCarta *temp = pilha->topo;
    Carta carta = temp->carta;
    pilha->topo = pilha->topo->proximo;
    free(temp);
    return carta;
}

// Necessário para saber se ainda existe alguma carta a ser utilizada. Haja visto que, ao usar a carta, ela não volta para a pilha
int semCartaNaPilha(Pilha *pilha)
{
    return pilha->topo == NULL;
}

// Enfileira os jogadores
void enfileirarJogador(Fila *fila, Jogador jogador)
{
    NoFila *novoNo = (NoFila *)malloc(sizeof(NoFila));
    novoNo->jogador = jogador;
    novoNo->proximo = NULL;
    if (fila->tras == NULL)
    {
        fila->frente = fila->tras = novoNo;
        return;
    }
    fila->tras->proximo = novoNo;
    fila->tras = novoNo;
}

// Essas duas funções são primordiais para definir a ordem dos jogadores

// Desenfileira os jogadores
Jogador desenfileirarJogador(Fila *fila)
{

    if (fila->frente == NULL)
    {
        printf("Fila vazia!\n");
        exit(1);
    }
    NoFila *temp = fila->frente;
    Jogador jogador = temp->jogador;
    fila->frente = fila->frente->proximo;
    if (fila->frente == NULL)
    {
        fila->tras = NULL;
    }
    free(temp);
    return jogador;
}

int fila_vazia(Fila *fila)
{
    return fila->frente == NULL;
}

// Função que simula o dado, retornando um número entre 1 e 6, de forma "aleatória"
int jogar_dado()
{
    int min = 1;
    int max = 6;
    int x = rand() % (max - min + 1) + min;
    printf("numero do dado: %d\n", x);
    return x;
}

void inicializar_tabuleiro(Casa tabuleiro[], int NUM_CASAS)
{
    // Inicializa as casas do tabuleiro baseado no parametro NUM_CASAS
    for (int i = 0; i < NUM_CASAS; i++)
    {
        tabuleiro[i].numero = i;
        tabuleiro[i].especial = 0;

        // Todas as casas múltiplas de 5 são consideradas especiais
        if (i % 5 == 0)
        {
            tabuleiro[i].especial = 1;
        }
        strcpy(tabuleiro[i].acao, "");
    }
}

void iniciarJogo(Fila *jogadores, Casa tabuleiro[], int NUM_CASAS, Pilha *cartas)
{
    while (1)
    {
        printf("---------------------------------\n");
        Jogador jogador = desenfileirarJogador(jogadores); // Desenfileira o jogador da fila

        if (jogador.pular_turno)
        {
            printf("%s esta pulando esta rodada.\n", jogador.nome);
            jogador.pular_turno = 0;
            enfileirarJogador(jogadores, jogador); // Enfileira o jogador novamente
            continue;
        }

        printf("Vez do jogador: %s\n", jogador.nome);
        printf("Pressione 'j' para jogar o dado: ");

        char input;
        do
        {
            input = getchar();
        } while (input != 'j');

        int dado = jogar_dado();
        printf("%s estava na casa %d\n", jogador.nome, jogador.posicao);
        jogador.posicao += dado;

        if (jogador.posicao >= NUM_CASAS)
        {
            printf("%s venceu o jogo!\n", jogador.nome);
            break;
        }

        printf("%s esta na casa %d\n", jogador.nome, tabuleiro[jogador.posicao].numero);

        if (tabuleiro[jogador.posicao].especial) // Se cair em uma casa especial, executa as ações abaixo
        {
            printf("caiu na casa especial. Executando a acao: %s\n", tabuleiro[jogador.posicao].acao);
            if (!semCartaNaPilha(cartas)) // Só executa a ação se houver cartas na pilha
            {
                Carta carta = desempilharCarta(cartas);
                printf("Carta tirada: %s\n", carta.descricao);
                jogador.posicao += carta.acao; // Com base na ação, move ou retrocede o jogador

                printf("%s foi parar na casa %d\n", jogador.nome, jogador.posicao);
                if (jogador.posicao < 0) // Se o jogador cair em uma casa negativa, ele volta para o início
                {
                    jogador.posicao = 0;
                }
                else if (jogador.posicao >= NUM_CASAS)
                {
                    jogador.posicao = NUM_CASAS - 1;
                }

                if (carta.acao == 0) // Se a ação for 0, fica uma rodada sem jogar
                {
                    jogador.pular_turno = 1;
                }
            }
            else
            {
                printf("Acabaram as cartas \n");
            }
        }
        // Caso não caia em uma casa especial, o jogador é movido e enfileirado novamente

        enfileirarJogador(jogadores, jogador);
    }
}

int main()
{
    srand(time(NULL)); // primo para gerar números aleatórios

#define NUM_CASAS 30 



    Casa tabuleiro[NUM_CASAS];
    inicializar_tabuleiro(tabuleiro, NUM_CASAS);

    // Cria a fila e a pilha
    Fila jogadores;
    jogadores.frente = jogadores.tras = NULL;

    // Define e enfileira os jogadores
    Jogador jogador1 = {"Jogador 1", 0, 0};
    enfileirarJogador(&jogadores, jogador1);

    Jogador jogador2 = {"Jogador 2", 0, 0};
    enfileirarJogador(&jogadores, jogador2);

    // Cria a pilha de cartas
    Pilha cartas;
    cartas.topo = NULL;

    Carta carta1 = {"Volte 3 casas", -3};
    empilharCarta(&cartas, carta1);

    Carta carta2 = {"Avance 2 casas", 2};
    empilharCarta(&cartas, carta2);

    Carta carta3 = {"Fique uma rodada sem jogar", 0};
    empilharCarta(&cartas, carta3);

    // Inicia o jogo
    iniciarJogo(&jogadores, tabuleiro, NUM_CASAS, &cartas);

    return 0;
}