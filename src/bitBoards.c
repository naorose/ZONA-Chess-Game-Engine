#include "bitBoards.h"


//Sourced from chess programming wiki
const int bitboard_table[64] = { 63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2, 51, 21, 43,
                                 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52, 26, 60, 6, 23, 44, 46,
                                 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8 };

int unset_bit(U64_t *bit_board) {
    U64_t bit = *bit_board ^ (*bit_board - 1);
    unsigned int value_to_fold = (unsigned) ((bit & HEX_MINUS_1) ^ (bit >> TWO_POWER_FIVE));
    *bit_board &= (*bit_board - 1);
    return bitboard_table[(value_to_fold * FOLD_MULTIPLIER) >> FOLD_SHIFT];
}

//Counts the number of bits that are set
int get_bits_count(U64_t bits) {
    int i;
    for (i = 0; bits; i++, bits &= bits - 1);
    return i;
}

void print_bit_board(U64_t bit_board) {
    U64_t value1 = 1ULL;
    int square = 0;
    int square64 = 0;
    for (int x = ROW_8; x >= ROW_1; --x) {
        for (int y = COLUMN_A; y <= COLUMN_H ; ++y) {
            square = convert_64_to_120(y, x);
            square64 = Base120ToBase64[square];
            if ((value1 << square64) & bit_board) {
                printf(" X ");
            } else {
                printf(" - ");
            }
        }
        printf("\n");
    }
}
