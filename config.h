/*
 * CONFIG.H - Configurações Gerais do Jogo
 * Local: shared/config.h
 */

#ifndef CONFIG_H
#define CONFIG_H

// Informações do trabalho
#define GAME_NAME "Jogo da Velha Multiplayer"
#define GAME_VERSION "1.0"
#define COURSE "Sistemas Operacionais - ESOFT-4S"
#define PROFESSOR "Prof. Maurilio Campano Jr"

// Configurações de visualização
#define CLEAR_SCREEN "cls"       // Windows
// #define CLEAR_SCREEN "clear"  // Linux/Mac

// Delays (milissegundos)
#define DELAY_ERROR_MSG 2000
#define DELAY_REFRESH 100
#define DELAY_INIT 1000

// Cores do terminal (Windows)
#ifdef _WIN32
    #define COLOR_RESET ""
    #define COLOR_RED ""
    #define COLOR_GREEN ""
    #define COLOR_YELLOW ""
    #define COLOR_BLUE ""
    #define COLOR_MAGENTA ""
    #define COLOR_CYAN ""
    #define COLOR_WHITE ""
#else
    #define COLOR_RESET "\033[0m"
    #define COLOR_RED "\033[31m"
    #define COLOR_GREEN "\033[32m"
    #define COLOR_YELLOW "\033[33m"
    #define COLOR_BLUE "\033[34m"
    #define COLOR_MAGENTA "\033[35m"
    #define COLOR_CYAN "\033[36m"
    #define COLOR_WHITE "\033[37m"
#endif

// Mensagens do jogo
#define MSG_WAITING_PLAYERS "Aguardando jogadores..."
#define MSG_GAME_START "Jogo iniciado!"
#define MSG_YOUR_TURN "É sua vez!"
#define MSG_OPPONENT_TURN "Aguardando adversário..."
#define MSG_YOU_WIN "VOCÊ VENCEU!"
#define MSG_YOU_LOSE "VOCÊ PERDEU!"
#define MSG_DRAW "EMPATE!"

// Debug mode (1 = ativo, 0 = desativo)
#define DEBUG_MODE 0

#if DEBUG_MODE
    #define DEBUG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

#endif // CONFIG_H
