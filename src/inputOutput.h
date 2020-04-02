
#ifndef ZONA_CHESS_ENGINE_INPUTOUTPUT_H
#define ZONA_CHESS_ENGINE_INPUTOUTPUT_H
#include <stdio.h>
#include "structures.h"
#include "load.h"
#include "moveGenerator.h"
#include "board.h"
#include "assertions.h"
char *print_move(const int move_to_print);
char *print_square(const int64_t square);
void print_move_tracker_list_no_score(const MOVES_TRACKERS_STRUCTURE *moves);
void print_move_tracker_list(const MOVES_TRACKERS_STRUCTURE *moves);
int translate_move(char *s, BOARD_STRUCTURE *pos);
#endif //ZONA_CHESS_ENGINE_INPUTOUTPUT_H
