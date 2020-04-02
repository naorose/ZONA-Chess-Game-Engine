#include "eval.h"

/*
 *
 *
 * decision tables were made and sourced with the help of online resources including chess wikis
 *
 *
 */

const int PawnDecisionTable[64] = {
        // positive values are good, negative values are bad
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
        10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
        5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
        0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
        5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
        10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
        20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
        // pawns promote eventually so to push them further, numbers increase down the board
};
const int BishopDecisionTable[64] = {
        0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
        0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
        0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
        0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
        0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
        0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
        0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};
const int KnightDecisionTable[64] = {
        // negative at start to try and advance the knights
        0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
        0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
        0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
        0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
        5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
        5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};
const int RookDecisionTable[64] = {
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0
};

const int otherSide[64] = {
        // gives us the equivalent of the tables above, but for black pieces, not white values.
        56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
        48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
        40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
        32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
        24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
        16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
        8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
        0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
};


// takes a board position and returns an integer which gives its evaluation score of position
int evaluate_pos(const BOARD_STRUCTURE *board) {
    int piece;
    int square;
    // we need to return positive from the point of view of the side to move, so white +ve, black -ve
    int output_score = board->material_value[W] - board->material_value[B];
    piece = whitePawn;
    for(int i = 0; i < board->piece_number[piece]; ++i) {
        square = board->piece_list[piece][i];
        ASSERT(is_square_on_board(square));
        output_score += PawnDecisionTable[Base120ToBase64[square]];
    }
    piece = blackPawn;
    for(int i = 0; i < board->piece_number[piece]; ++i) {
        square = board->piece_list[piece][i];
        ASSERT(is_square_on_board(square));
        output_score -= PawnDecisionTable[otherSide[Base120ToBase64[square]]];
    }
    piece = whiteKnight;
    for(int i = 0; i < board->piece_number[piece]; ++i) {
        square = board->piece_list[piece][i];
        ASSERT(is_square_on_board(square));
        output_score += KnightDecisionTable[Base120ToBase64[square]];
    }
    piece = blackKnight;
    for(int i = 0; i < board->piece_number[piece]; ++i) {
        square = board->piece_list[piece][i];
        ASSERT(is_square_on_board(square));
        output_score -= KnightDecisionTable[otherSide[Base120ToBase64[square]]];
    }
    piece = whiteBishop;
    for(int i = 0; i < board->piece_number[piece]; ++i) {
        square = board->piece_list[piece][i];
        ASSERT(is_square_on_board(square));
        output_score += BishopDecisionTable[Base120ToBase64[square]];
    }
    piece = blackBishop;
    for(int i = 0; i < board->piece_number[piece]; ++i) {
        square = board->piece_list[piece][i];
        ASSERT(is_square_on_board(square));
        output_score -= BishopDecisionTable[otherSide[Base120ToBase64[square]]];
    }
    piece = whiteRook;
    for(int i = 0; i < board->piece_number[piece]; ++i) {
        square = board->piece_list[piece][i];
        ASSERT(is_square_on_board(square));
        output_score += RookDecisionTable[Base120ToBase64[square]];
    }
    piece = blackRook;
    for(int i = 0; i < board->piece_number[piece]; ++i) {
        square = board->piece_list[piece][i];
        ASSERT(is_square_on_board(square));
        output_score -= RookDecisionTable[otherSide[Base120ToBase64[square]]];
    }
    // negates score if it is black to move
    if(board->turn == W) {
        return output_score;
    } else {
        return -output_score;
    }
}
