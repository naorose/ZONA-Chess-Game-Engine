#ifndef CHESSENGINE_STRUCTURES_H
#define CHESSENGINE_STRUCTURES_H
#include <stdlib.h>

//Debugging tool gotten from Beginning Programming in C++
//#define DEBUG
#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed",#n); \
printf("On %s ",__DATE__); \
printf("At %s ",__TIME__); \
printf("In File %s ",__FILE__); \
printf("At Line %d\n",__LINE__); \
exit(1);}
#endif

#define NAME "Zona 1.0"
#define BOARD_SIZE 120
#define NUM_COLOURS 2
#define NUM_OF_PIECES 13
#define MAX_GAME_MOVES 2048
#define ACTUAL_BOARD_SIZE 64
#define MAX_NUM_SAME_PIECE 10
#define CASTLE_INV_VALUE 16
#define MAXIMUM_DEPTH 64

#define MAX_NO_MOVES_POSITION 256 //may change to 512
#define NO_MOVE 0
#define INIT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

typedef unsigned long long U64_t;

typedef enum {
    NULL_LOCATION,
    whitePawn,
    whiteKnight,
    whiteBishop,
    whiteRook,
    whiteQueen,
    whiteKing,
    blackPawn,
    blackKnight,
    blackBishop,
    blackRook,
    blackQueen,
    blackKing
} pieceType;

typedef enum {
    ROW_1,
    ROW_2,
    ROW_3,
    ROW_4,
    ROW_5,
    ROW_6,
    ROW_7,
    ROW_8,
    ROW_NULL
} row;

typedef enum {
    COLUMN_A,
    COLUMN_B,
    COLUMN_C,
    COLUMN_D,
    COLUMN_E,
    COLUMN_F,
    COLUMN_G,
    COLUMN_H,
    COLUMN_NULL
} column;

typedef enum {
    W,
    B,
    B_AND_W
} colour;

enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQUARE, OUT_OF_BOUNDS
};

typedef enum {
    F,
    T
} bool;

typedef enum {
    WHITEKINGCASTLING = 1,
    WHITEQUEENCASTLING = 2,
    BLACKKINGCASTLING = 4,
    BLACKQUEENCASTLING = 8
} castling;

typedef struct {
    U64_t positionKey;
    int move;
}PVENTRY_STRUCTURE;

typedef struct {
    PVENTRY_STRUCTURE *pTable;
    int numEntries;
} PVTABLE_STRUCTURE;

typedef struct {
    int move;
    int score;
} MOVE_STRUCTURE;

//Allows for keeping track of game status as time of each move
typedef struct {
    int move;
    int castlePermission;
    int enPassant;
    int fiftyMoveCounter;
    U64_t positionKey;
} UNDO_STRUCTURE;

typedef struct {
    float failHigh;
    float failHighFirst;
    int initTime;
    int endTime;
    int depth;
    int depth_set;
    int time_set;  // e.g. 5 minutes for all moves
    int movesLeft;
    int infinite;
    long positionsVisited;
    int quit; // if interrupted during a search and GUI sends 'quit', this is set to true and program is closed
    int ended; // when set to true we recursively back straight out of the search without updating any values
} SEARCH_INFO_STRUCTURE;

typedef struct {
    MOVE_STRUCTURE moveTracker[MAX_NO_MOVES_POSITION];
    int count;
} MOVES_TRACKERS_STRUCTURE;

typedef struct {
    int board_squares[BOARD_SIZE];
    U64_t pawns[NUM_COLOURS + 1];
    int kingSquare[2];
    int turn;
    int enPassant;
    int fiftyMoveCounter;
    int halfMovesPlayedCurrent;
    int halfMovesPlayedGame;
    int castling_permission;
    U64_t position_key;
    int piece_number[NUM_OF_PIECES];
    int nonPawnPieces[NUM_COLOURS];
    int major_board_pieces[NUM_COLOURS]; //Queen and Rook
    int minor_board_pieces[NUM_COLOURS]; //Knight and Pawns
    int material_value[NUM_COLOURS];
    UNDO_STRUCTURE game_history[MAX_GAME_MOVES];

    //list of pieces
    int piece_list[NUM_OF_PIECES][MAX_NUM_SAME_PIECE];

    PVTABLE_STRUCTURE pvTable[1];
    int PV_list[MAXIMUM_DEPTH];

    int search_previous_values[NUM_OF_PIECES][BOARD_SIZE];
    // store the two moves which have most recently caused a beta cutoff which aren't kills
    int search_killers[NUM_COLOURS][MAXIMUM_DEPTH];
} BOARD_STRUCTURE;

//Transalting the index from total board to actual board and vice versa
extern int Base120ToBase64[BOARD_SIZE];
extern int Base64ToBase120[ACTUAL_BOARD_SIZE];
extern U64_t setMask[64];
extern U64_t unsetMask[64];
extern U64_t piece_values[NUM_OF_PIECES][BOARD_SIZE];
extern U64_t side_value;
extern U64_t castling_values[CASTLE_INV_VALUE];
extern char pieceCharacters[];
extern char turn_characters[];
extern char rankCharacters[];
extern char fileCharacters[];

//Contains data describing each piece
extern int non_pawn_pieces[NUM_OF_PIECES];
extern int major_pieces[NUM_OF_PIECES];
extern int minor_pieces[NUM_OF_PIECES];
extern int piece_value[NUM_OF_PIECES];
extern int piece_colour[NUM_OF_PIECES];

extern int BoardColumns[BOARD_SIZE];
extern int BoardRows[BOARD_SIZE];

extern int is_piece_knight[NUM_OF_PIECES];
extern int is_piece_king[NUM_OF_PIECES];
extern int is_piece_rook_queen[NUM_OF_PIECES];
extern int is_piece_bishop_queen[NUM_OF_PIECES];
extern int is_piece_pawn[NUM_OF_PIECES];
extern int is_sliding_piece[NUM_OF_PIECES];

#endif //CHESSENGINE_STRUCTURES_H