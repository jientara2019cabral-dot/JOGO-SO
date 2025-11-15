#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#include "../shared/protocol.h"
#include "../shared/config.h"

SOCKET sock;
int my_player_num = 0;
char my_symbol = ' ';

void display_board(char* board) {
    system(CLEAR_SCREEN);
    
    printf("\n=== %s ===\n", GAME_NAME);
    printf("Você: Jogador %d (%c)\n\n", my_player_num, my_symbol);
    
    printf(" %c | %c | %c \n", board[0], board[1], board[2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", board[3], board[4], board[5]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n\n", board[6], board[7], board[8]);
}

void parse_message(char* buffer) {
    if(strncmp(buffer, MSG_PLAYER, strlen(MSG_PLAYER)) == 0) {
        sscanf(buffer, "PLAYER:%d:%c", &my_player_num, &my_symbol);
        printf("Você é o Jogador %d (%c)\n", my_player_num, my_symbol);
    }
    else if(strncmp(buffer, MSG_STATE, strlen(MSG_STATE)) == 0) {
        char board[10];
        int current_player, game_over, winner;
        sscanf(buffer, "STATE:%9[^:]:%d:%d:%d", board, &current_player, &game_over, &winner);
        
        display_board(board);
        
        if(game_over) {
            if(winner == my_player_num) {
                printf("🎉 %s 🎉\n", MSG_YOU_WIN);
            } else if(winner == 0) {
                printf("🤝 %s\n", MSG_DRAW);
            } else {
                printf("😞 %s\n", MSG_YOU_LOSE);
            }
        } else {
            if(current_player == my_player_num) {
                printf("✅ %s\n", MSG_YOUR_TURN);
                printf("Digite a posição (0-8): ");
            } else {
                printf("⏳ %s\n", MSG_OPPONENT_TURN);
            }
        }
    }
    else if(strncmp(buffer, MSG_ERROR, strlen(MSG_ERROR)) == 0) {
        printf("❌ Jogada inválida! Tente novamente.\n");
    }
}

int main() {
    WSADATA wsa;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    
    printf("=== CLIENTE %s ===\n", GAME_NAME);
    
    // Inicializar Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // Configurar conexão
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(PORT);
    
    if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Erro ao conectar ao servidor!\n");
        return 1;
    }
    
    printf("Conectado ao servidor! Aguardando início do jogo...\n");
    
    while(1) {
        // Receber mensagem do servidor
        memset(buffer, 0, BUFFER_SIZE);
        int recv_size = recv(sock, buffer, BUFFER_SIZE, 0);
        
        if(recv_size <= 0) {
            printf("Conexão perdida com o servidor!\n");
            break;
        }
        
        buffer[recv_size] = '\0';
        parse_message(buffer);
        
        // Se for minha vez, ler jogada
        if(strncmp(buffer, MSG_STATE, strlen(MSG_STATE)) == 0) {
            char board[10];
            int current_player, game_over, winner;
            sscanf(buffer, "STATE:%9[^:]:%d:%d:%d", board, &current_player, &game_over, &winner);
            
            if(!game_over && current_player == my_player_num) {
                int position;
                scanf("%d", &position);
                
                format_move_msg(buffer, position);
                send(sock, buffer, strlen(buffer), 0);
            }
        }
    }
    
    closesocket(sock);
    WSACleanup();
    return 0;
}
