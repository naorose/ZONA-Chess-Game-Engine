#include <stdio.h>
#include "structures.h"
#include "load.h"
#include "bitBoards.h"
#include "hashValues.h"
#include "board.h"
#include "attack.h"
#include "inputOutput.h"
#include "moveGenerator.h"
#include "moveMaker.h"
#include "perft.h"
#include "search.h"
#include "eval.h"
#include "miscellaneous.h"
#include "assertions.h"
#include "data.h"
#include "uci.h"

#define demo1queensonly "8/3q4/8/8/4Q3/8/8/8 w - - 0 2"
#define demo1queensandpawns "8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 2"
#define demo1pawnsonly "8/3p4/8/8/4P3/8/8/8 w - - 0 2"


#define demo2 "8/3k1p2/8/5P2/4K3/8/8/8 w - - 0 2"
#define demo2nopawns "8/3k4/8/8/4K3/8/8/8 w - - 0 2"

#define demo3 "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"

#define fen1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define fen2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define fen3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define fen4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define fen5 "8/3q4/8/8/4Q3/8/8/8 w - - 0 2 "
#define fen6 "8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 2 "
#define KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define ROOKS "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1"
#define QUEENS "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1 "
#define BISHOPS "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1 "
#define WAC2 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define PAWNMOVESW "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVESB "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define CASTLE1 "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
#define CASTLE2 "3rk2r/8/8/8/8/8/6p1/R3K2R b KQk - 0 1"
#define CASTLE3 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define PERFTFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"
#define WAC1 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"

void showSquares(const int side, const BOARD_STRUCTURE *board) {
    int row = 0;
    int column = 0;
    int square = 0;
    printf("\n\nSquares attacked by: %c\n",turn_characters[side]);
    for(row = ROW_8; row >= ROW_1; --row) {
        for (column = COLUMN_A; column <= COLUMN_H; ++column) {
            square = convert_64_to_120(column, row);
            if (square_in_danger(square, side, board) == T) {
                printf(" X ");
            } else {
                printf(" - ");
            }
        }
        printf("\n");
    }
    printf("\n\n");
}

//int main() {
//    return 0;
//}


//// demonstration #1
//// shows that we have successfully calculated the places on the board in which each piece can attack
//int main() {
//    loadBoard();
//    BOARD_STRUCTURE board[1];
//    translate_FEN(demo1queensonly, board); // as you can see by this demo, the Queen has a lot of power in chess
////    translate_FEN(demo1pawnsonly, board);
//    print_board(board);
//    printf("\nWhite attacking:");
//    showSquares(W, board);
//    printf("Black attacking:");
//    showSquares(B, board);
//    return 0;
//}


//// demonstration #2 - promoting chess pieces
//int main() {
//    loadBoard();
//    int move = 0;
//    // choice of H7 due to where the pawn will be about to promote
//    int from = A3; int to = H7; int piece = whitePawn; int promoteTo = whiteQueen;
////    int from = A3; int to = H7; int piece = whitePawn; int promoteTo = whiteRook;
////    int from = A3; int to = H7; int piece = whitePawn; int promoteTo = blackQueen; // rules of promotion vary
////    int from = A3; int to = H7; int piece = whitePawn; int promoteTo = whiteKing; // should not give king, should give queen
////    int from = A3; int to = H7; int piece = whitePawn; int promoteTo = whiteBishop;
//    move = ( ( from ) | ( to << 7 ) | ( piece << 14 ) | ( promoteTo << 20) );
//    printf("From: %s\n",print_square(from));
//    printf("To: %s\n",print_square(to));
//    printf("Promotion move: %s\n",print_move(move)); // the letter on the end represents what it promoted to
//    return 0;
//}


//// demonstration #3 - calculating all possible moves
//int main() {
//    loadBoard();
//    BOARD_STRUCTURE board[1];
//    translate_FEN(demo3,board);
//    print_board(board);
//    MOVES_TRACKERS_STRUCTURE list[1];
//    generate_all_moves(board,list);
//    print_move_tracker_list_no_score(list);
//    return 0;
//}


// demonstration #4 - identifying move types
int main() {
    loadBoard();
    BOARD_STRUCTURE board[1];
    MOVES_TRACKERS_STRUCTURE list[1];
    translate_FEN(KNIGHTSKINGS,board);
    generate_all_moves(board,list);
    return 0;
}