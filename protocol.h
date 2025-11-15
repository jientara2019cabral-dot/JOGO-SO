#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PORT 8888
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 256
#define BOARD_SIZE 9
#define MAX_PLAYERS 2

// Códigos de mensagem
#define MSG_PLAYER "PLAYER"
#define MSG_MOVE "MOVE"
#define MSG_STATE "STATE"
#define MSG_ERROR "ERROR"
#define MSG_GET_STATE "GET_STATE"

// Códigos de erro
#define ERR_NOT_YOUR_TURN -1
#define ERR_INVALID_POS -2
#define ERR_POS_OCCUPIED -3

// Estados do jogo
#define GAME_PLAYING 0
#define GAME_WIN 1
#define GAME_DRAW 2

// Símbolos
#define SYMBOL_P1 'X'
#define SYMBOL_P2 'O'

// Estrutura do estado do jogo
typedef struct {
    char board[BOARD_SIZE];
    int current_player;
    int game_over;
    int winner;
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
    sprintf(buffer, "%s:%s:%d:%d:%d", MSG_STATE, board, current_player, game_over, winner);
}

static inline void format_error_msg(char* buffer, int error_code) {
    sprintf(buffer, "%s:%d", MSG_ERROR, error_code);
}

// Validação
static inline int is_valid_position(int pos) {
    return pos >= 0 && pos < BOARD_SIZE;
}

#endif
