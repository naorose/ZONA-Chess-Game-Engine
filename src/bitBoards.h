#ifndef CHESSENGINE_BITBOARDS_H
#define CHESSENGINE_BITBOARDS_H
#define HEX_MINUS_1 -1
#define TWO_POWER_FIVE 32
#define FOLD_MULTIPLIER 0x783a9b23
#define FOLD_SHIFT 26
#include <stdio.h>
#include "structures.h"
#include "load.h"
void print_bit_board(U64_t bit_board);
int unset_bit(U64_t *bit_board);
int get_bits_count(U64_t bits);

#endif //CHESSENGINE_BITBOARDS_H