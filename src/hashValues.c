
#include "hashValues.h"
U64_t initialise_position_key(const BOARD_STRUCTURE *board) {
    U64_t output_key = 0;
    int board_piece = NULL_LOCATION;
    for(int i = 0; i < BOARD_SIZE; ++i) {
        board_piece = board->board_squares[i];
        if((board_piece != NO_SQUARE) && (board_piece != NULL_LOCATION) && (board_piece!=OUT_OF_BOUNDS)) {
            ASSERT(board_piece >= whitePawn && board_piece <= blackKing);
            output_key ^= piece_values[board_piece][i];
        }
    }
    if(board->turn == W) {
        output_key ^= side_value;
    }
    if(board->enPassant != NO_SQUARE) {
        ASSERT(board->enPassant>=0 && board->enPassant<BOARD_SIZE);
        output_key ^= piece_values[NULL_LOCATION][board->enPassant];
    }
    ASSERT(board->castling_permission>=0 && board->castling_permission<=15);
    output_key ^= castling_values[board->castling_permission];
    return output_key;
}