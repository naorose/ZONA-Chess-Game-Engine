#include "load.h"

int Base120ToBase64[BOARD_SIZE];
int Base64ToBase120[ACTUAL_BOARD_SIZE];

U64_t setMask[64];
U64_t unsetMask[64];

U64_t piece_values[NUM_OF_PIECES][BOARD_SIZE];
U64_t side_value;
U64_t castling_values[CASTLE_INV_VALUE];

int BoardColumns[BOARD_SIZE];
int BoardRows[BOARD_SIZE];

// Initialises the board's rows and columns
void load_rows_columns_board() {
    int row = ROW_1;
    int column = COLUMN_A;
    int square = A1;
    int square64 = 0;

    // loop through every position in array and set them as out of bounds
    for(int i = 0; i < BOARD_SIZE; ++i) {
        BoardColumns[i] = OUT_OF_BOUNDS;
        BoardRows[i] = OUT_OF_BOUNDS;
    }

    // loop which goes through all columns and rows and setting each square to its respective column and row
    for(row = ROW_1; row <= ROW_8; ++row) {
        for(column = COLUMN_A; column <= COLUMN_H; ++column) {
            square = convert_64_to_120(column, row);
            BoardColumns[square] = column;
            BoardRows[square] = row;
        }
    }

//    printf("Columns Board\n");
//    for (int i = 0; i < BOARD_SIZE; ++i) {
//        if (i % 10 == 0 && i != 0) {
//            printf("\n");
//        }
//        printf("%4d", BoardColumns[i]);
//    }
//
//    printf("Rows Board\n");
//    for (int i = 0; i < BOARD_SIZE; ++i) {
//        if (i % 10 == 0 && i != 0) {
//            printf("\n");
//        }
//        printf("%4d", BoardRows[i]);
//    }
}



//Converts Base 64 position to Base 120 position
int convert_64_to_120(int column, int row) {
    return ((21 + column) + (row * 10));
}

//Initialiase both arrays properly
void load_arrays() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        Base120ToBase64[i] = 65;
    }

    for (int i = 0; i < ACTUAL_BOARD_SIZE; ++i) {
        Base64ToBase120[i] = 120;
    }

    int index = 0;
    for (int x = ROW_1; x <= ROW_8; ++x) {
        for (int y = COLUMN_A; y <= COLUMN_H; ++y) {
            Base64ToBase120[index] = convert_64_to_120(y, x);
            Base120ToBase64[convert_64_to_120(y, x)] = index;
            index++;
        }
    }
}

void load_bit_masks() {
    for (int i = 0; i < ACTUAL_BOARD_SIZE; ++i) {
        setMask[i] = 0ULL;
        unsetMask[i] = 0ULL;
    }

    for (int i = 0; i < ACTUAL_BOARD_SIZE; ++i) {
        setMask[i] |= (1ULL << i);
        unsetMask[i] = ~setMask[i];
    }
}

void set_bit(U64_t *bitBoard, U64_t location) {
    *bitBoard |= setMask[location];
}

void clear_bit(U64_t *bitBoard, U64_t location) {
    *bitBoard &= unsetMask[location];
}

//Rand generates 15 bit numbers, we need to generate a 64 bit number
U64_t generate_random_U64() {
    return  ((U64_t)rand()) |
            ((U64_t)rand() << 15) |
            ((U64_t)rand << 30) |
            ((U64_t)rand << 45) |
            (((U64_t)rand & 0XF) << 60);
}

//Initialise the hash arrays
void load_hash_keys() {
    for(int i = 0; i < NUM_OF_PIECES; ++i) {
        for(int j = 0; j < BOARD_SIZE; ++j) {
            piece_values[i][j] = generate_random_U64();

        }
    }
    side_value = generate_random_U64();
    for(int x = 0; x < CASTLE_INV_VALUE; ++x) {
        castling_values[x] = generate_random_U64();
    }
}

void loadBoard() {
    load_arrays();
    load_bit_masks();
    load_hash_keys();
    load_rows_columns_board();
    load_MVV_LVA();
}

//Move instruction breakdown
int from_square(uint32_t m) {
    return (m & 0X7F);
}

int to_square(uint32_t m) {
    return ((m >> 7) & 0X7F);
}

int piece_attacked(uint32_t m) {
    return ((m >> 14) & 0XF);
}

int EN_PASSANT_FLAG = 0X40000; // either make everything a constant or make everything a function. v inconsistent.

int is_pawn_start() {
    return (0X80000);
}

int promotePiece(uint32_t m) {
    return ((m >> 20) & 0XF);
}

int is_castling() {
    return (0X1000000);
}

int CAPTURING_MOVE_FLAG = (0X7C000);

int isPromotionMove() {
    return (0XF00000);
}