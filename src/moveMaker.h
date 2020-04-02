#ifndef ZONA_CHESS_ENGINE_MOVEMAKER_H
#define ZONA_CHESS_ENGINE_MOVEMAKER_H

#include "structures.h"
#include "load.h"
#include "assertions.h"
#include "board.h"
#include "attack.h"

#define ENPASSANT_DIST 10
int make_move(BOARD_STRUCTURE *board, int move);
int move_exists(BOARD_STRUCTURE *board, const int move);
static void add_to_board(const int square, BOARD_STRUCTURE *board, const int piece);
static void move_board_piece(const int fromLocation, const int toLocation, BOARD_STRUCTURE *board);
static void remove_from_board(const int square, BOARD_STRUCTURE *board);
void hashSide(BOARD_STRUCTURE *board);
void hash_board_piece(int piece, int square, BOARD_STRUCTURE *board);
void hash_castling_permission(BOARD_STRUCTURE *board);
void hash_enpassant(BOARD_STRUCTURE *board);
void take_back_move(BOARD_STRUCTURE *board);
#endif //ZONA_CHESS_ENGINE_MOVEMAKER_H
