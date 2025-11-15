/*
 * PROTOCOL.H - Definições do Protocolo de Comunicação
 * Compartilhado entre Servidor e Cliente
 * Local: shared/protocol.h
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

// Configurações de rede
#define PORT 8888
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 256
#define MAX_PLAYERS 2

// Tamanho do tabuleiro
#define BOARD_SIZE 9

// Códigos de mensagem
#define MSG_PLAYER "PLAYER"      // Servidor informa número do jogador
#define MSG_MOVE "MOVE"          // Cliente envia jogada
#define MSG_STATE "STATE"        // Servidor envia estado do jogo
#define MSG_ERROR "ERROR"        // Servidor envia erro
#define MSG_GET_STATE "GET_STATE" // Cliente solicita estado

// Códigos de erro
#define ERR_NOT_YOUR_TURN -1     // Não é seu turno
#define ERR_INVALID_POS -2       // Posição inválida
#define ERR_POS_OCCUPIED -3      // Posição já ocupada

// Estados do jogo
#define GAME_PLAYING 0           // Jogo em andamento
#define GAME_WIN 1               // Alguém venceu
#define GAME_DRAW 2              // Empate

// Símbolos dos jogadores
#define SYMBOL_P1 'X'
#define SYMBOL_P2 'O'

// Estrutura de memória compartilhada (para o servidor)
typedef struct {
    char board[BOARD_SIZE];      // Tabuleiro 3x3
    int current_player;          // 1 ou 2
    int game_over;               // 0=jogando, 1=vitória, 2=empate
    int winner;                  // 0=nenhum, 1=jogador1, 2=jogador2
} GameState;

// Funções auxiliares para mensagens
static inline void format_player_msg(char* buffer, int player_num, char symbol) {
    sprintf(buffer, "%s:%d:%c", MSG_PLAYER, player_num, symbol);
}

static inline void format_move_msg(char* buffer, int position) {
    sprintf(buffer, "%s:%d", MSG_MOVE, position);
}

static inline void format_state_msg(char* buffer, const char* board, 
                                   int current_player, int game_over, int winner) {
    sprintf(buffer, "%s:%s:%d:%d:%d", MSG_STATE, board, 
            current_player, game_over, winner);
}

static inline void format_error_msg(char* buffer, int error_code) {
    sprintf(buffer, "%s:%d", MSG_ERROR, error_code);
}

// Validação de posição
static inline int is_valid_position(int pos) {
    return pos >= 0 && pos < BOARD_SIZE;
}

#endif // PROTOCOL_H
