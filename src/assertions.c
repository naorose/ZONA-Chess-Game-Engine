#include "assertions.h"

int is_square_on_board(const int square) {
    return BoardColumns[square] != OUT_OF_BOUNDS;
}
int is_turn_valid(const int turn) {
    return (turn==W || turn==B);
}
int is_column_row_valid(const int column_row) {
    return (column_row >= COLUMN_A && column_row <= COLUMN_H);
}
int is_piece_valid(const int piece) {
    return (piece >= whitePawn && piece <= blackKing);
}
int is_piece_valid_w_empty(const int piece) {
    return (piece >= NULL_LOCATION && piece <= blackKing);
}
