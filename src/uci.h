#ifndef ZONA_CHESS_ENGINE_UCI_H
#define ZONA_CHESS_ENGINE_UCI_H

#include "structures.h"
#include "search.h"

void translate_go_uci(char *input_line, SEARCH_INFO_STRUCTURE *information, BOARD_STRUCTURE *board);
void translate_position_uci(char *input_line, BOARD_STRUCTURE *board);
void uci_loop();
#define MAX_INPUT_STRING 400 * UCI_VALUE6
#define NEG -1
#define NEUT 0
#define POS 1
#define UCI_VALUE9 9
#define UCI_VALUE8 8
#define UCI_VALUE6 6
#define UCI_VALUE2 2
#define UCI_VALUE3 3
#define UCI_VALUE4 4
#define UCI_VALUE10 10

#endif //ZONA_CHESS_ENGINE_UCI_H