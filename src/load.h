#ifndef CHESSENGINE_LOAD_H
#define CHESSENGINE_LOAD_H

#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "moveGenerator.h"

U64_t generate_random_U64();
int CAPTURING_MOVE_FLAG;
int EN_PASSANT_FLAG;
int convert_64_to_120(int column, int row);
int from_square(uint32_t m);
int isPromotionMove(uint32_t m);
int is_castling();
int is_pawn_start();
int piece_attacked(uint32_t m);
int promotePiece(uint32_t m);
int to_square(uint32_t m);
void clear_bit(U64_t *bitBoard, U64_t location);
void loadBoard();
void load_arrays();
void load_bit_masks();
void load_hash_keys();
void load_rows_columns_board();
void set_bit(U64_t *bitBoard, U64_t location);

#endif //CHESSENGINE_LOAD_H
