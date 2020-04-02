
#include "attack.h"
const int bishop_directions[4] = { -9, -11, 11, 9 }; // diagonal
const int king_directions[8] = { -1, -10,1, 10, -9, -11, 11, 9 }; // can move 1 square in any direction
const int knight_directions[8] = { -8, -19,-21, -12, 8, 19, 21, 12 }; // 2 forward 1 to side, 1 forward 2 to side (L)
const int rook_directions[4] = { -1, -10,1, 10 };

int square_in_danger(const int square, const int turn, const BOARD_STRUCTURE *board) {
    int direction;
    int piece;
    int temporary_square;
    ASSERT(is_square_on_board(square)); // checks if square is on board
    ASSERT(is_turn_valid(turn));     // checks if the turn is either W or B
    ASSERT(run_board_check(board));     // ensures board has been properly initialised
    // Pawns
    if (turn == W) { // if attacking side is white
        if (board->board_squares[square - PAWN_DIAGONAL_ATTACK_1] == whitePawn ||
            board->board_squares[square - PAWN_DIAGONAL_ATTACK_2] == whitePawn) {
            return T;
        }
    } else { // if attacking side is black
        if (board->board_squares[square + PAWN_DIAGONAL_ATTACK_1] == blackPawn ||
            board->board_squares[square + PAWN_DIAGONAL_ATTACK_2] == blackPawn) {
            return T;
        }
    }
    //Knights can move in an L shape
    for (int i = 0; i < KNIGHT_ATTACK_1; ++i) {
        piece = board->board_squares[square + knight_directions[i]];
        if (piece != OUT_OF_BOUNDS && is_piece_knight[piece] && piece_colour[piece] == turn) {
            return T;
        }
    }
    // Rooks and Queens (Rooks can move H and V. Queens can move H, V and D
    for(int i = 0; i < 4; ++i) {
        direction = rook_directions[i];
        temporary_square = square + direction;
        piece = board->board_squares[temporary_square];
        while(piece != OUT_OF_BOUNDS) {
            if(piece != NULL_LOCATION) {
                if(is_piece_rook_queen[piece] && piece_colour[piece] == turn) {
                    return T;
                }
                break;
            }
            temporary_square += direction;
            piece = board->board_squares[temporary_square];
        }
    }
    // Bishops and Queens (Bishops can move D, Queen can move H, V, D)
    for(int i = 0; i < 4; ++i) {
        direction = bishop_directions[i];
        temporary_square = square + direction;
        piece = board->board_squares[temporary_square];
        while(piece != OUT_OF_BOUNDS) {
            if(piece != NULL_LOCATION) {
                if(is_piece_bishop_queen[piece] && piece_colour[piece] == turn) {
                    return T;
                }
                break;
            }
            temporary_square += direction;
            piece = board->board_squares[temporary_square];
        }
    }
    // Kings can move H, V, D but only one distance
    for(int i = 0; i < 8; ++i) {
        piece = board->board_squares[square + king_directions[i]];
        if(piece != OUT_OF_BOUNDS && is_piece_king[piece] && piece_colour[piece]==turn) {
            return T;
        }
    }
    return F;
}