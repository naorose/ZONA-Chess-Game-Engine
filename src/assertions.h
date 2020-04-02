

#include "structures.h"
#ifndef ZONA_CHESS_ENGINE_ASSERTIONS_H
#define ZONA_CHESS_ENGINE_ASSERTIONS_H
int is_square_on_board(const int square);
int is_turn_valid(const int turn);
int is_column_row_valid(const int column_row);
int is_piece_valid_w_empty(const int piece);
int is_piece_valid(const int piece);
#endif //ZONA_CHESS_ENGINE_ASSERTIONS_H
