/*
 * SERVER.C - Servidor do Jogo da Velha
 * Local: server/server.c
 * 
 * Compilar: gcc server.c -o ../bin/server.exe -lws2_32 -I../shared
 * Executar: cd bin && server.exe
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

// Estrutura de memória compartilhada com Critical Section
typedef struct {
    GameState state;
    CRITICAL_SECTION cs;
} SharedMemory;

SharedMemory* sharedMem;
SOCKET client1 = INVALID_SOCKET;
SOCKET client2 = INVALID_SOCKET;
HANDLE hMapFile;

// Protótipos de funções
void init_board();
void display_board();
int check_winner();
int process_move(int player, int position);
unsigned __stdcall client_handler(void* arg);

// Função para inicializar o tabuleiro
void init_board() {
    EnterCriticalSection(&sharedMem->cs);
    
    for(int i = 0; i < BOARD_SIZE; i++) {
        sharedMem->state.board[i] = '0' + i; // '0' a '8'
    }
    sharedMem->state.current_player = 1;
    sharedMem->state.game_over = GAME_PLAYING;
    sharedMem->state.winner = 0;
    
    LeaveCriticalSection(&sharedMem->cs);
    
    DEBUG_PRINT("Tabuleiro inicializado");
}

// Função para exibir o tabuleiro no servidor
void display_board() {
    printf("\n=== TABULEIRO ATUAL ===\n");
    printf(" %c | %c | %c \n", 
           sharedMem->state.board[0], 
           sharedMem->state.board[1], 
           sharedMem->state.board[2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", 
           sharedMem->state.board[3], 
           sharedMem->state.board[4], 
           sharedMem->state.board[5]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", 
           sharedMem->state.board[6], 
           sharedMem->state.board[7], 
           sharedMem->state.board[8]);
    printf("=======================\n");
    printf("Turno: Jogador %d\n\n", sharedMem->state.current_player);
}

// Verificar vitória ou empate
int check_winner() {
    char* board = sharedMem->state.board;
    
    // Verificar linhas
    for(int i = 0; i < 9; i += 3) {
        if(board[i] == board[i+1] && board[i+1] == board[i+2]) {
            return board[i] == SYMBOL_P1 ? 1 : 2;
        }
    }
    
    // Verificar colunas
    for(int i = 0; i < 3; i++) {
        if(board[i] == board[i+3] && board[i+3] == board[i+6]) {
            return board[i] == SYMBOL_P1 ? 1 : 2;
        }
    }
    
    // Verificar diagonais
    if(board[0] == board[4] && board[4] == board[8]) {
        return board[0] == SYMBOL_P1 ? 1 : 2;
    }
    if(board[2] == board[4] && board[4] == board[6]) {
        return board[2] == SYMBOL_P1 ? 1 : 2;
    }
    
    // Verificar empate (tabuleiro cheio)
    int full = 1;
    for(int i = 0; i < BOARD_SIZE; i++) {
        if(board[i] != SYMBOL_P1 && board[i] != SYMBOL_P2) {
            full = 0;
            break;
        }
    }
    if(full) return -1; // Empate
    
    return 0; // Jogo continua
}

// Processar jogada de um jogador
int process_move(int player, int position) {
    EnterCriticalSection(&sharedMem->cs);
    
    DEBUG_PRINT("Processando jogada: Jogador %d, Posição %d", player, position);
    
    // Verificar se é o turno correto
    if(sharedMem->state.current_player != player) {
        LeaveCriticalSection(&sharedMem->cs);
        DEBUG_PRINT("Erro: Não é o turno do jogador %d", player);
        return ERR_NOT_YOUR_TURN;
    }
    
    // Verificar se posição é válida
    if(!is_valid_position(position)) {
        LeaveCriticalSection(&sharedMem->cs);
        DEBUG_PRINT("Erro: Posição inválida %d", position);
        return ERR_INVALID_POS;
    }
    
    // Verificar se posição está livre
    if(sharedMem->state.board[position] == SYMBOL_P1 || 
       sharedMem->state.board[position] == SYMBOL_P2) {
        LeaveCriticalSection(&sharedMem->cs);
        DEBUG_PRINT("Erro: Posição %d já ocupada", position);
        return ERR_POS_OCCUPIED;
    }
    
    // Fazer jogada
    sharedMem->state.board[position] = (player == 1) ? SYMBOL_P1 : SYMBOL_P2;
    
    // Verificar resultado
    int result = check_winner();
    if(result > 0) {
        sharedMem->state.game_over = GAME_WIN;
        sharedMem->state.winner = result;
        DEBUG_PRINT("Jogador %d venceu!", result);
    } else if(result == -1) {
        sharedMem->state.game_over = GAME_DRAW;
        DEBUG_PRINT("Empate!");
    } else {
        // Trocar turno
        sharedMem->state.current_player = (player == 1) ? 2 : 1;
        DEBUG_PRINT("Turno alterado para jogador %d", sharedMem->state.current_player);
    }
    
    LeaveCriticalSection(&sharedMem->cs);
    return 0; // Jogada válida
}

// Thread para gerenciar cada cliente
unsigned __stdcall client_handler(void* arg) {
    SOCKET client = *(SOCKET*)arg;
    int player_num = (client == client1) ? 1 : 2;
    char buffer[BUFFER_SIZE];
    char symbol = (player_num == 1) ? SYMBOL_P1 : SYMBOL_P2;
    
    printf("Cliente %d conectado! Símbolo: %c\n", player_num, symbol);
    
    // Enviar número do jogador
    format_player_msg(buffer, player_num, symbol);
    send(client, buffer, strlen(buffer), 0);
    
    DEBUG_PRINT("Enviado para jogador %d: %s", player_num, buffer);
    
    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_size = recv(client, buffer, BUFFER_SIZE, 0);
        
        if(recv_size <= 0) {
            printf("Cliente %d desconectou\n", player_num);
            break;
        }
        
        DEBUG_PRINT("Recebido do jogador %d: %s", player_num, buffer);
        
        // Processar mensagem MOVE
        if(strncmp(buffer, MSG_MOVE, strlen(MSG_MOVE)) == 0) {
            int position = atoi(buffer + strlen(MSG_MOVE) + 1);
            printf("Jogador %d jogou na posição %d\n", player_num, position);
            
            int result = process_move(player_num, position);
            
            if(result == 0) {
                // Jogada válida
                display_board();
                
                // Enviar estado atualizado para ambos os clientes
                EnterCriticalSection(&sharedMem->cs);
                format_state_msg(buffer, 
                                sharedMem->state.board,
                                sharedMem->state.current_player,
                                sharedMem->state.game_over,
                                sharedMem->state.winner);
                LeaveCriticalSection(&sharedMem->cs);
                
                send(client1, buffer, strlen(buffer), 0);
                send(client2, buffer, strlen(buffer), 0);
                
                DEBUG_PRINT("Estado enviado para ambos os jogadores");
            } else {
                // Enviar erro
                format_error_msg(buffer, result);
                send(client, buffer, strlen(buffer), 0);
                DEBUG_PRINT("Erro enviado para jogador %d: %d", player_num, result);
            }
        }
        // Processar mensagem GET_STATE
        else if(strncmp(buffer, MSG_GET_STATE, strlen(MSG_GET_STATE)) == 0) {
            EnterCriticalSection(&sharedMem->cs);
            format_state_msg(buffer,
                            sharedMem->state.board,
                            sharedMem->state.current_player,
                            sharedMem->state.game_over,
                            sharedMem->state.winner);
            LeaveCriticalSection(&sharedMem->cs);
            
            send(client, buffer, strlen(buffer), 0);
            DEBUG_PRINT("Estado enviado para jogador %d", player_num);
        }
    }
    
    closesocket(client);
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET server_socket;
    struct sockaddr_in server, client;
    int c;
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║  %s  ║\n", GAME_NAME);
    printf("║         SERVIDOR - v%s             ║\n", GAME_VERSION);
    printf("╚════════════════════════════════════════╝\n\n");
    
    // Criar memória compartilhada
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(SharedMemory),
        "JogoDaVelhaSharedMem"
    );
    
    if(hMapFile == NULL) {
        printf("Erro ao criar memória compartilhada: %d\n", GetLastError());
        return 1;
    }
    
    sharedMem = (SharedMemory*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0, 0,
        sizeof(SharedMemory)
    );
    
    if(sharedMem == NULL) {
        printf("Erro ao mapear memória: %d\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }
    
    InitializeCriticalSection(&sharedMem->cs);
    init_board();
    
    printf("✓ Memória compartilhada inicializada\n");
    
    // Inicializar Winsock
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Erro ao inicializar Winsock: %d\n", WSAGetLastError());
        return 1;
    }
    
    printf("✓ Winsock inicializado\n");
    
    // Criar socket
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Erro ao criar socket: %d\n", WSAGetLastError());
        return 1;
    }
    
    printf("✓ Socket criado\n");
    
    // Configurar endereço
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    
    // Bind
    if(bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Erro no bind: %d\n", WSAGetLastError());
        return 1;
    }
    
    printf("✓ Bind realizado na porta %d\n", PORT);
    
    // Listen
    listen(server_socket, MAX_PLAYERS);
    printf("\n%s\n", MSG_WAITING_PLAYERS);
    
    c = sizeof(struct sockaddr_in);
    
    // Aceitar jogador 1
    printf("Aguardando Jogador 1...\n");
    client1 = accept(server_socket, (struct sockaddr*)&client, &c);
    if(client1 == INVALID_SOCKET) {
        printf("Erro ao aceitar conexão do Jogador 1\n");
        return 1;
    }
    printf("✓ Jogador 1 conectado!\n\n");
    
    // Aceitar jogador 2
    printf("Aguardando Jogador 2...\n");
    client2 = accept(server_socket, (struct sockaddr*)&client, &c);
    if(client2 == INVALID_SOCKET) {
        printf("Erro ao aceitar conexão do Jogador 2\n");
        return 1;
    }
    printf("✓ Jogador 2 conectado!\n\n");
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║         %s         ║\n", MSG_GAME_START);
    printf("╚════════════════════════════════════════╝\n");
    
    display_board();
    
    // Criar threads para cada cliente
    HANDLE thread1 = (HANDLE)_beginthreadex(NULL, 0, client_handler, &client1, 0, NULL);
    HANDLE thread2 = (HANDLE)_beginthreadex(NULL, 0, client_handler, &client2, 0, NULL);
    
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);
    
    // Limpeza
    printf("\nEncerrando servidor...\n");
    DeleteCriticalSection(&sharedMem->cs);
    UnmapViewOfFile(sharedMem);
    CloseHandle(hMapFile);
    closesocket(server_socket);
    WSACleanup();
    
    printf("Servidor encerrado.\n");
    return 0;
}
