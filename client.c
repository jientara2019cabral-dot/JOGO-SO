/*
 * CLIENT.C - Cliente do Jogo da Velha
 * Local: client/client.c
 * 
 * Compilar: gcc client.c -o ../bin/client.exe -lws2_32 -I../shared
 * Executar: cd bin && client.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

// Incluir arquivos compartilhados
#include "../shared/protocol.h"
#include "../shared/config.h"

// Variáveis globais do cliente
SOCKET sock;
int my_player_num = 0;
char my_symbol = ' ';
GameState game_state;

// Protótipos de funções
void display_board();
void parse_state(char* message);
unsigned __stdcall receive_messages(void* arg);

// Função para exibir o tabuleiro
void display_board() {
    system(CLEAR_SCREEN);
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║    %s    ║\n", GAME_NAME);
    printf("╚════════════════════════════════════════╝\n\n");
    
    printf("Você: Jogador %d (%c)  |  Adversário: Jogador %d (%c)\n", 
           my_player_num, my_symbol,
           (my_player_num == 1) ? 2 : 1,
           (my_player_num == 1) ? SYMBOL_P2 : SYMBOL_P1);
    
    printf("\n     |     |     \n");
    printf("  %c  |  %c  |  %c  \n", 
           game_state.board[0], 
           game_state.board[1], 
           game_state.board[2]);
    printf("_____|_____|_____\n");
    printf("     |     |     \n");
    printf("  %c  |  %c  |  %c  \n",
