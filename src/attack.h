#ifndef ZONA_CHESS_ENGINE_ATTACK_H
#define ZONA_CHESS_ENGINE_ATTACK_H

#include "data.h"
#include "structures.h"
#include "assertions.h"
#include "board.h"
#define PAWN_DIAGONAL_ATTACK_1 11
#define PAWN_DIAGONAL_ATTACK_2 9
#define KNIGHT_ATTACK_1 8
int square_in_danger(const int square, const int turn, const BOARD_STRUCTURE *board);

#endif //ZONA_CHESS_ENGINE_ATTACK_H
