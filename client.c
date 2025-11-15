#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")
#include "../shared/protocol.h"

SOCKET sock;
int my_player = 0;
char my_symbol = ' ';

void display_board(char* board) {
    system("cls");
    printf("\n=== JOGO DA VELHA ===\n");
    printf("Voce: Jogador %d (%c)\n\n", my_player, my_symbol);
    
    printf(" %c | %c | %c \n", board[0], board[1], board[2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", board[3], board[4], board[5]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n\n", board[6], board[7], board[8]);
}

void parse_message(char* buffer) {
    if(strncmp(buffer, "PLAYER:", 7) == 0) {
        sscanf(buffer, "PLAYER:%d:%c", &my_player, &my_symbol);
        printf("Voce e o Jogador %d (%c)\n", my_player, my_symbol);
    }
    else if(strncmp(buffer, "STATE:", 6) == 0) {
        char board[10];
        int turn, over, winner;
        sscanf(buffer, "STATE:%9[^:]:%d:%d:%d", board, &turn, &over, &winner);
        
        display_board(board);
        
        if(over) {
            if(winner == my_player) printf("*** VOCE VENCEU! ***\n");
            else if(winner == 0) printf("*** EMPATE! ***\n");
            else printf("*** VOCE PERDEU! ***\n");
        } else {
            if(turn == my_player) printf("Sua vez! Digite posicao (0-8): ");
            else printf("Aguardando adversario...\n");
        }
    }
    else if(strncmp(buffer, "ERROR:", 6) == 0) {
        printf "Jogada invalida! Tente novamente.\n");
    }
}

int main() {
    WSADATA wsa;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    
    printf("=== CLIENTE JOGO DA VELHA ===\n");
    
    WSAStartup(MAKEWORD(2,2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8888);
    
    if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Erro ao conectar!\n");
        return 1;
    }
    
    printf("Conectado ao servidor!\n");
    
    while(1) {
        // Receber mensagens do servidor
        memset(buffer, 0, BUFFER_SIZE);
        int recv_size = recv(sock, buffer, BUFFER_SIZE, 0);
        if(recv_size > 0) {
            parse_message(buffer);
            
            // Se for minha vez, ler jogada
            if(strncmp(buffer, "STATE:", 6) == 0) {
                char board[10];
                int turn, over, winner;
                sscanf(buffer, "STATE:%9[^:]:%d:%d:%d", board, &turn, &over, &winner);
                
                if(!over && turn == my_player) {
                    int pos;
                    printf("Sua jogada (0-8): ");
                    scanf("%d", &pos);
                    
                    format_move_msg(buffer, pos);
                    send(sock, buffer, strlen(buffer), 0);
                }
            }
        } else {
            break;
        }
    }
    
    closesocket(sock);
    WSACleanup();
    return 0;
}
