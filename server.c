#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#include "../shared/protocol.h"
#include "../shared/config.h"

// Estrutura com memória compartilhada
typedef struct {
    GameState state;
    CRITICAL_SECTION cs;
} SharedMemory;

SharedMemory* sharedMem;
SOCKET client1 = INVALID_SOCKET;
SOCKET client2 = INVALID_SOCKET;
HANDLE hMapFile;

void init_board() {
    EnterCriticalSection(&sharedMem->cs);
    
    for(int i = 0; i < BOARD_SIZE; i++) {
        sharedMem->state.board[i] = '0' + i;
    }
    sharedMem->state.current_player = 1;
    sharedMem->state.game_over = GAME_PLAYING;
    sharedMem->state.winner = 0;
    
    LeaveCriticalSection(&sharedMem->cs);
    printf("Tabuleiro inicializado\n");
}

void display_board() {
    printf("\n=== TABULEIRO ATUAL ===\n");
    printf(" %c | %c | %c \n", sharedMem->state.board[0], sharedMem->state.board[1], sharedMem->state.board[2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", sharedMem->state.board[3], sharedMem->state.board[4], sharedMem->state.board[5]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", sharedMem->state.board[6], sharedMem->state.board[7], sharedMem->state.board[8]);
    printf("Turno: Jogador %d\n\n", sharedMem->state.current_player);
}

int check_winner() {
    char* board = sharedMem->state.board;
    
    // Linhas
    for(int i = 0; i < 9; i += 3) {
        if(board[i] == board[i+1] && board[i+1] == board[i+2]) {
            return board[i] == SYMBOL_P1 ? 1 : 2;
        }
    }
    
    // Colunas
    for(int i = 0; i < 3; i++) {
        if(board[i] == board[i+3] && board[i+3] == board[i+6]) {
            return board[i] == SYMBOL_P1 ? 1 : 2;
        }
    }
    
    // Diagonais
    if(board[0] == board[4] && board[4] == board[8]) {
        return board[0] == SYMBOL_P1 ? 1 : 2;
    }
    if(board[2] == board[4] && board[4] == board[6]) {
        return board[2] == SYMBOL_P1 ? 1 : 2;
    }
    
    // Empate
    int full = 1;
    for(int i = 0; i < BOARD_SIZE; i++) {
        if(board[i] != SYMBOL_P1 && board[i] != SYMBOL_P2) {
            full = 0;
            break;
        }
    }
    if(full) return -1;
    
    return 0;
}

int process_move(int player, int position) {
    EnterCriticalSection(&sharedMem->cs);
    
    // Verificar turno
    if(sharedMem->state.current_player != player) {
        LeaveCriticalSection(&sharedMem->cs);
        return ERR_NOT_YOUR_TURN;
    }
    
    // Verificar posição
    if(!is_valid_position(position)) {
        LeaveCriticalSection(&sharedMem->cs);
        return ERR_INVALID_POS;
    }
    
    // Verificar se está livre
    if(sharedMem->state.board[position] == SYMBOL_P1 || 
       sharedMem->state.board[position] == SYMBOL_P2) {
        LeaveCriticalSection(&sharedMem->cs);
        return ERR_POS_OCCUPIED;
    }
    
    // Fazer jogada
    sharedMem->state.board[position] = (player == 1) ? SYMBOL_P1 : SYMBOL_P2;
    
    // Verificar resultado
    int result = check_winner();
    if(result > 0) {
        sharedMem->state.game_over = GAME_WIN;
        sharedMem->state.winner = result;
    } else if(result == -1) {
        sharedMem->state.game_over = GAME_DRAW;
    } else {
        sharedMem->state.current_player = (player == 1) ? 2 : 1;
    }
    
    LeaveCriticalSection(&sharedMem->cs);
    return 0;
}

unsigned __stdcall client_handler(void* arg) {
    SOCKET client = *(SOCKET*)arg;
    int player_num = (client == client1) ? 1 : 2;
    char buffer[BUFFER_SIZE];
    char symbol = (player_num == 1) ? SYMBOL_P1 : SYMBOL_P2;
    
    printf("Cliente %d conectado! Símbolo: %c\n", player_num, symbol);
    
    // Enviar identificação do jogador
    format_player_msg(buffer, player_num, symbol);
    send(client, buffer, strlen(buffer), 0);
    
    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_size = recv(client, buffer, BUFFER_SIZE, 0);
        
        if(recv_size <= 0) {
            printf("Cliente %d desconectou\n", player_num);
            break;
        }
        
        // Processar MOVE
        if(strncmp(buffer, MSG_MOVE, strlen(MSG_MOVE)) == 0) {
            int position = atoi(buffer + strlen(MSG_MOVE) + 1);
            printf("Jogador %d jogou na posição %d\n", player_num, position);
            
            int result = process_move(player_num, position);
            
            if(result == 0) {
                // Jogada válida - enviar estado para ambos
                display_board();
                
                EnterCriticalSection(&sharedMem->cs);
                format_state_msg(buffer, sharedMem->state.board,
                                sharedMem->state.current_player,
                                sharedMem->state.game_over,
                                sharedMem->state.winner);
                LeaveCriticalSection(&sharedMem->cs);
                
                send(client1, buffer, strlen(buffer), 0);
                send(client2, buffer, strlen(buffer), 0);
            } else {
                // Enviar erro
                format_error_msg(buffer, result);
                send(client, buffer, strlen(buffer), 0);
            }
        }
    }
    
    closesocket(client);
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET server_socket;
    struct sockaddr_in server;
    
    printf("=== SERVIDOR %s ===\n", GAME_NAME);
    
    // Criar memória compartilhada
    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                                0, sizeof(SharedMemory), "JogoDaVelhaSharedMem");
    sharedMem = (SharedMemory*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
    InitializeCriticalSection(&sharedMem->cs);
    init_board();
    
    printf("Memória compartilhada inicializada\n");
    
    // Inicializar Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    // Configurar servidor
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    
    bind(server_socket, (struct sockaddr*)&server, sizeof(server));
    listen(server_socket, MAX_PLAYERS);
    
    printf("\n%s\n", MSG_WAITING_PLAYERS);
    
    // Aceitar jogadores
    printf("Aguardando Jogador 1...\n");
    client1 = accept(server_socket, NULL, NULL);
    printf("✓ Jogador 1 conectado!\n");
    
    printf("Aguardando Jogador 2...\n");
    client2 = accept(server_socket, NULL, NULL);
    printf("✓ Jogador 2 conectado!\n");
    
    printf("\n%s\n", MSG_GAME_START);
    display_board();
    
    // Criar threads para os clientes
    HANDLE thread1 = _beginthreadex(NULL, 0, client_handler, &client1, 0, NULL);
    HANDLE thread2 = _beginthreadex(NULL, 0, client_handler, &client2, 0, NULL);
    
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);
    
    // Limpeza
    printf("Encerrando servidor...\n");
    DeleteCriticalSection(&sharedMem->cs);
    UnmapViewOfFile(sharedMem);
    CloseHandle(hMapFile);
    closesocket(server_socket);
    WSACleanup();
    
    return 0;
}
