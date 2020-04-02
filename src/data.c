
#include "data.h"

char pieceCharacters[] = ".PNBRQKpnbrqk";
char turn_characters[] = "wb-";
char rankCharacters[] = "12345678";
char fileCharacters[] = "abcdefgh";

int major_pieces[NUM_OF_PIECES] = { F, F, F, F, T, T, T, F, F, F, T, T, T };
int minor_pieces[NUM_OF_PIECES] = { F, F, T, T, F, F, F, F, T, T, F, F, F };
int non_pawn_pieces[NUM_OF_PIECES] = { F, F, T, T, T, T, T, F, T, T, T, T, T };
int piece_colour[NUM_OF_PIECES] = { B_AND_W, W, W, W, W, W, W, B, B, B, B, B, B };
int piece_value[NUM_OF_PIECES] = { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000 };

// true set whenever the piece is a bishop or queen
int is_piece_bishop_queen[NUM_OF_PIECES] = { F, F, F, T, F, T, F, F, F, T, F, T, F };
// true set whenever the piece is a king
int is_piece_king[NUM_OF_PIECES] = { F, F, F, F, F, F, T, F, F, F, F, F, T };
// true set whenever the piece is a knight
int is_piece_knight[NUM_OF_PIECES] = { F, F, T, F, F, F, F, F, T, F, F, F, F };
// true set whenever the piece is a pawn
int is_piece_pawn[NUM_OF_PIECES] = { F, T, F, F, F, F, F, T, F, F, F, F, F };
// true set whenever the piece is a rook or a queen
int is_piece_rook_queen[NUM_OF_PIECES] = { F, F, F, F, T, T, F, F, F, F, T, T, F };
// true set whenever the piece is a sliding piece
int is_sliding_piece[NUM_OF_PIECES] = { F, F, F, T, T, T, F, F, F, T, T, T, F };