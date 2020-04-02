#ifndef ZONA_CHESS_ENGINE_BOARD_H
#define ZONA_CHESS_ENGINE_BOARD_H
#include <stdio.h>
#include "structures.h"
#include "load.h"
#include "hashValues.h"
#include "bitBoards.h"
#include "search.h"

#define NULL_MOVE 0;
#define FEN_JUMP_COLOUR_CASTLING 2
#define TWO_POWER_THREE 8
void reset_board(BOARD_STRUCTURE *board);
int translate_FEN(char *fen_string, BOARD_STRUCTURE *board);
void print_board(const BOARD_STRUCTURE *board);
void update_piece_values_lists(BOARD_STRUCTURE *board);
int run_board_check(const BOARD_STRUCTURE *board);
BOARD_STRUCTURE* generate_board();

#endif //ZONA_CHESS_ENGINE_BOARD_H