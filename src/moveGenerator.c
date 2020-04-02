
#include "moveGenerator.h"


#define PAWN_MOVE_1 10
#define PAWN_MOVE_2 20
#define SLIDING_PIECES_WHITE_INDEX 0
#define SLIDING_PIECES_BLACK_INDEX 4
#define NON_SLIDING_PIECES_WHITE_INDEX 0
#define NON_SLIDING_PIECES_BLACK_INDEX 3
#define DIRECTION_INCREMENTS 8
#define SCORE_ADDITION 6
#define SCORE_DIVISION 100
#define EN_PASSANT_SCORE 105
#define CAPTURE_SCORE_BASE 1000000


const int PieceDirectionList[NUM_OF_PIECES][DIRECTION_INCREMENTS] = {
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0 },
        { -8, -19,	-21, -12, 8, 19, 21, 12 },
        { -9, -11, 11, 9, 0, 0, 0, 0 },
        { -1, -10,	1, 10, 0, 0, 0, 0 },
        { -1, -10,	1, 10, -9, -11, 11, 9 },
        { -1, -10,	1, 10, -9, -11, 11, 9 },
        { 0, 0, 0, 0, 0, 0, 0 },
        { -8, -19,	-21, -12, 8, 19, 21, 12 },
        { -9, -11, 11, 9, 0, 0, 0, 0 },
        { -1, -10,	1, 10, 0, 0, 0, 0 },
        { -1, -10,	1, 10, -9, -11, 11, 9 },
        { -1, -10,	1, 10, -9, -11, 11, 9 }
};


/* Normally when you order moves inside a Chess Search tree in alpha beta is as follows:
 * search principal variation moves if present (PV moves)
 * search kills (MVV LVA)
 * search beta-cut offs
 * search according to history score (incremented when they improve alpha) */

/* Most valuable CAPTURES (from high to low) :
 * pawn takes queen (highest)
 * knight takes queen
 * bishop takes queen
 * ...
 * pawn takes rook
 * knight takes rook
 * ...
 * queen takes pawn (lowest) */

// when attackee is Queen score is 500, Rook is 400 etc.
const int attackeeScore[NUM_OF_PIECES] = {0,100,200,300,400,500,600,100,200,300,400,500,600};

static int MVVLVAScoring[NUM_OF_PIECES][NUM_OF_PIECES];

int load_MVV_LVA() {
    int attacker;
    int attackee;
    for(attacker = whitePawn; attacker <= blackKing; ++attacker) {
        for(attackee = whitePawn; attackee <= blackKing; ++attackee) {
            MVVLVAScoring[attackee][attacker] = attackeeScore[attackee] + SCORE_ADDITION -
                    ( attackeeScore[attacker] / SCORE_DIVISION);
        }
    }

    for(attackee = whitePawn; attackee <= blackKing; ++attackee) {
        for(attacker = whitePawn; attacker <= blackKing; ++attacker) {
//            printf("%c attacking %c  would give score of: %d\n",pieceCharacters[attacker],
//                    pieceCharacters[attackee], MVVLVAScoring[attackee][attacker]);
        }
    }
    // *** TO SILENCE WARNING ***
    return 0;
    // *** TO SILENCE WARNING ***
}

// checking whether a possible move exists or not
int move_exists(BOARD_STRUCTURE *board, const int move) {
    MOVES_TRACKERS_STRUCTURE trackerStructure[1];
    generate_all_moves(board, trackerStructure);
    for(int i = 0; i < trackerStructure->count; ++i) {
        if (!make_move(board, trackerStructure->moveTracker[i].move))  {
            continue;
        }
        take_back_move(board);
        // if move does exist on the board
        if(trackerStructure->moveTracker[i].move == move) {
            return T;
        }
    }
    return F;
}

const int NumberOfDirections[NUM_OF_PIECES] = {NO_MOVES, NO_MOVES, EIGHT_MOVES, FOUR_MOVES,
                                               FOUR_MOVES, EIGHT_MOVES, EIGHT_MOVES, NO_MOVES,
                                               EIGHT_MOVES, FOUR_MOVES, FOUR_MOVES, EIGHT_MOVES, EIGHT_MOVES};

const int SlidingPiecesLoop[8] = { // changed NULLs to 0s
    whiteBishop, whiteRook, whiteQueen, 0, // loops from 0 until it hits NULL
    blackBishop, blackRook, blackQueen, 0  // loops from 4 until it hits NULL
};

const int SlidingPiecesIndex[NUM_COLOURS] = {
    SLIDING_PIECES_WHITE_INDEX, SLIDING_PIECES_BLACK_INDEX
};

const int NonSlidingPiecesLoop[6] = { //changed NULLs to 0s
    whiteKnight, whiteKing, 0,
    blackKnight, blackKing, 0
};

const int NonSlidingPiecesIndex[NUM_COLOURS] = {
    NON_SLIDING_PIECES_WHITE_INDEX, NON_SLIDING_PIECES_BLACK_INDEX
};

int create_move_int(int f, int t, int ca, int pro, int fl) {
    return (f | (t << 7) | (ca << 14) | (pro << PAWN_MOVE_2) | fl);
}

int is_square_off_board(int square) {
    return BoardColumns[square] == OUT_OF_BOUNDS;
}

//moveGeneration will take the board and a moveTracker list and will loop through all board_squares

static void add_nonkill_move(const BOARD_STRUCTURE *board, int m, MOVES_TRACKERS_STRUCTURE *trackerStructure) {
    ASSERT(is_square_on_board(from_square(m)));
    ASSERT(is_square_on_board(to_square(m)));
    trackerStructure->moveTracker[trackerStructure->count].move = m;
    if (board->search_killers[0][board->halfMovesPlayedCurrent] == m) {
        trackerStructure->moveTracker[trackerStructure->count].score = 900000;
    } else if (board->search_killers[1][board->halfMovesPlayedCurrent] == m) {
        trackerStructure->moveTracker[trackerStructure->count].score = 800000;
    } else {
        trackerStructure->moveTracker[trackerStructure->count].score =
                board->search_previous_values[board->board_squares[from_square(m)]][to_square(m)];
    }
    trackerStructure->count++;
}

static void add_kill_move(const BOARD_STRUCTURE *board, int m, MOVES_TRACKERS_STRUCTURE *trackerStructure) {
    ASSERT(is_square_on_board(from_square(m)));
    ASSERT(is_square_on_board(to_square(m)));
    ASSERT(is_square_on_board(piece_attacked(m)));
    trackerStructure->moveTracker[trackerStructure->count].move = m;
    trackerStructure->moveTracker[trackerStructure->count].score =
            MVVLVAScoring[piece_attacked(m)][board->board_squares[from_square(m)]] + CAPTURE_SCORE_BASE;
    trackerStructure->count++;
}

static void add_enpassant_move(const BOARD_STRUCTURE *board, int m, MOVES_TRACKERS_STRUCTURE *trackerStructure) {
    ASSERT(is_square_on_board(from_square(m)));
    ASSERT(is_square_on_board(to_square(m)));
    trackerStructure->moveTracker[trackerStructure->count].move = m;
    trackerStructure->moveTracker[trackerStructure->count].score = EN_PASSANT_SCORE + CAPTURE_SCORE_BASE;
    trackerStructure->count++;
}

static void add_wP_kill_move( const BOARD_STRUCTURE *board, const int from, const int to, const int cap, MOVES_TRACKERS_STRUCTURE *list ) {
    ASSERT(is_piece_valid_w_empty(cap));
    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));
    ASSERT(run_board_check(board));
    if(BoardRows[from] == ROW_7) {
        add_kill_move(board, create_move_int(from, to, cap, whiteQueen, 0), list);
        add_kill_move(board, create_move_int(from, to, cap, whiteRook, 0), list);
        add_kill_move(board, create_move_int(from, to, cap, whiteBishop, 0), list);
        add_kill_move(board, create_move_int(from, to, cap, whiteKnight, 0), list);
    } else {
        add_kill_move(board, create_move_int(from, to, cap, NULL_LOCATION, 0), list);
    }
}

static void add_wP_nonkill_move( const BOARD_STRUCTURE *board, const int from, const int to, MOVES_TRACKERS_STRUCTURE *trackerStructure ) {
    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));
    ASSERT(run_board_check(board));
    if(BoardRows[from] == ROW_7) { // if pawn is on 7th row means that it's going to the 8th row so it's promoting
        add_nonkill_move(board, create_move_int(from, to, NULL_LOCATION, whiteQueen, 0), trackerStructure);
        add_nonkill_move(board, create_move_int(from, to, NULL_LOCATION, whiteRook, 0), trackerStructure);
        add_nonkill_move(board, create_move_int(from, to, NULL_LOCATION, whiteBishop, 0), trackerStructure);
        add_nonkill_move(board, create_move_int(from, to, NULL_LOCATION, whiteKnight, 0), trackerStructure);
    } else { // if it's not on the 7th row then it is not promoting.
        add_nonkill_move(board, create_move_int(from, to, NULL_LOCATION, NULL_LOCATION, 0), trackerStructure);
    }
}

static void add_bP_kill_move( const BOARD_STRUCTURE *board, const int from, const int to, const int cap, MOVES_TRACKERS_STRUCTURE *list ) {
    ASSERT(is_piece_valid_w_empty(cap));
    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));
    ASSERT(run_board_check(board));
    if(BoardRows[from] == ROW_2) {
        add_kill_move(board, create_move_int(from, to, cap, blackQueen, 0), list);
        add_kill_move(board, create_move_int(from, to, cap, blackRook, 0), list);
        add_kill_move(board, create_move_int(from, to, cap, blackBishop, 0), list);
        add_kill_move(board, create_move_int(from, to, cap, blackKnight, 0), list);
    } else {
        add_kill_move(board, create_move_int(from, to, cap, NULL_LOCATION, 0), list);
    }
}

static void add_bP_nonkill_move( const BOARD_STRUCTURE *board, const int from, const int to, MOVES_TRACKERS_STRUCTURE *list ) {
    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));
    ASSERT(run_board_check(board));
    if(BoardRows[from] == ROW_2) {
        add_nonkill_move(board, create_move_int(from, to, NULL_LOCATION, blackQueen, 0), list);
        add_nonkill_move(board, create_move_int(from, to, NULL_LOCATION, blackRook, 0), list);
        add_nonkill_move(board, create_move_int(from, to, NULL_LOCATION, blackBishop, 0), list);
        add_nonkill_move(board, create_move_int(from, to, NULL_LOCATION, blackKnight, 0), list);
    } else {
        add_nonkill_move(board, create_move_int(from, to, NULL_LOCATION, NULL_LOCATION, 0), list);
    }
}

void generate_all_moves_old(const BOARD_STRUCTURE *board, MOVES_TRACKERS_STRUCTURE *trackerStructure) {
    ASSERT(run_board_check(board));
    int piece = NULL_LOCATION;
    int turn = board->turn;
    int square = 0;
    int targetSquare = 0;
    int direction = 0;
    int piece_i = 0;
    if (turn == W) { // we want to loop through each one of our white pawns on the board

        for (int i = 0; i < board->piece_number[whitePawn]; ++i) {
            square = board->piece_list[whitePawn][i];
            ASSERT(is_square_on_board(square)) // ensure square that it is giving us is on the board
            //non kill moves
            if (board->board_squares[square + PAWN_MOVE_1] == NULL_LOCATION) {
                add_wP_nonkill_move(board, square, square + PAWN_MOVE_1, trackerStructure);
                if (BoardRows[square] == ROW_2 && board->board_squares[square + PAWN_MOVE_2] == NULL_LOCATION) {
                    add_nonkill_move(board, create_move_int(square, (square + PAWN_MOVE_2), NULL_LOCATION, NULL_LOCATION,
                                                            is_pawn_start()),
                                     trackerStructure);
                }
            }
            if (!is_square_off_board(square + PAWN_DIAGONAL_ATTACK_2) && piece_colour[board->board_squares[square + PAWN_DIAGONAL_ATTACK_2]] == B) {
                add_wP_kill_move(board, square, square + PAWN_DIAGONAL_ATTACK_2, board->board_squares[square + PAWN_DIAGONAL_ATTACK_2], trackerStructure);
            }
            if (!is_square_off_board(square + PAWN_DIAGONAL_ATTACK_1) && piece_colour[board->board_squares[square + PAWN_DIAGONAL_ATTACK_1]] == B) {
                add_wP_kill_move(board, square, square + PAWN_DIAGONAL_ATTACK_1, board->board_squares[square + PAWN_DIAGONAL_ATTACK_1], trackerStructure);
            }

            if (board->enPassant != NO_SQUARE) {
                if (square + PAWN_DIAGONAL_ATTACK_2 == board->enPassant) {
                    add_enpassant_move(board, create_move_int(square, square + PAWN_DIAGONAL_ATTACK_2, NULL_LOCATION, NULL_LOCATION,
                                                              EN_PASSANT_FLAG), trackerStructure);
                }
                if (square + PAWN_DIAGONAL_ATTACK_1 == board->enPassant) {
                    add_enpassant_move(board, create_move_int(square, square + PAWN_DIAGONAL_ATTACK_1, NULL_LOCATION, NULL_LOCATION,
                                                              EN_PASSANT_FLAG), trackerStructure);
                }
            }

        }


        //CASTLING CARRIED OUT IF POSSIBLE
        if (board->castling_permission & WHITEKINGCASTLING) {
            if (board->board_squares[F1] == NULL_LOCATION && board->board_squares[G1] == NULL_LOCATION) {
                if (!square_in_danger(E1, B, board) && !square_in_danger(F1, B, board)) {
                    add_nonkill_move(board, create_move_int(E1, G1, NULL_LOCATION, NULL_LOCATION, is_castling()), trackerStructure);
                }
            }
        }

        if (board->castling_permission & WHITEQUEENCASTLING) {
            if (board->board_squares[D1] == NULL_LOCATION && board->board_squares[C1] == NULL_LOCATION && board->board_squares[B1] == NULL_LOCATION) {
                if (!square_in_danger(E1, B, board) && !square_in_danger(D1, B, board)) {
                    add_nonkill_move(board, create_move_int(E1, C1, NULL_LOCATION, NULL_LOCATION, is_castling()), trackerStructure);
                }
            }
        }

    } else {

        for(int i = 0; i < board->piece_number[blackPawn]; ++i) {
            square = board->piece_list[blackPawn][i];
            ASSERT(is_square_on_board(square));

            if(board->board_squares[square - PAWN_MOVE_1] == NULL_LOCATION) {
                add_bP_nonkill_move(board, square, square-PAWN_MOVE_1, trackerStructure);
                if(BoardRows[square] == ROW_7 && board->board_squares[square - PAWN_MOVE_2] == NULL_LOCATION) {
                    add_nonkill_move(board, create_move_int(square, (square - PAWN_MOVE_2), NULL_LOCATION, NULL_LOCATION,
                                                            is_pawn_start()),trackerStructure);
                }
            }
            // black attacking white
            if(!is_square_off_board(square - PAWN_DIAGONAL_ATTACK_2) && piece_colour[board->board_squares[square - PAWN_DIAGONAL_ATTACK_2]] == W) {
                add_bP_kill_move(board, square, square-PAWN_DIAGONAL_ATTACK_2, board->board_squares[square - PAWN_DIAGONAL_ATTACK_2], trackerStructure);
            }
            if(!is_square_off_board(square - PAWN_DIAGONAL_ATTACK_1) && piece_colour[board->board_squares[square - PAWN_DIAGONAL_ATTACK_1]] == W) {
                add_bP_kill_move(board, square, square-PAWN_DIAGONAL_ATTACK_1, board->board_squares[square - PAWN_DIAGONAL_ATTACK_1], trackerStructure);
            }
            if (board->enPassant != NO_SQUARE) {
                if (square - PAWN_DIAGONAL_ATTACK_2 == board->enPassant) {
                    add_enpassant_move(board,
                                       create_move_int(square, square - PAWN_DIAGONAL_ATTACK_2, NULL_LOCATION, NULL_LOCATION,
                                                       EN_PASSANT_FLAG), trackerStructure);
                }
                if (square - PAWN_DIAGONAL_ATTACK_1 == board->enPassant) {
                    add_enpassant_move(board,
                                       create_move_int(square, square - PAWN_DIAGONAL_ATTACK_1, NULL_LOCATION, NULL_LOCATION,
                                                       EN_PASSANT_FLAG), trackerStructure);
                }
            }
        }

        //CASTLING EXECUTION
        if (board->castling_permission & BLACKKINGCASTLING) {
            if (board->board_squares[F8] == NULL_LOCATION && board->board_squares[G8] == NULL_LOCATION) {
                if (!square_in_danger(E8, W, board) && !square_in_danger(F8, W, board)) {
                    add_nonkill_move(board, create_move_int(E8, G8, NULL_LOCATION, NULL_LOCATION, is_castling()), trackerStructure);
                }
            }
        }

        if (board->castling_permission & WHITEQUEENCASTLING) {
            if (board->board_squares[D8] == NULL_LOCATION && board->board_squares[C8] == NULL_LOCATION && board->board_squares[B8] == NULL_LOCATION) {
                if (!square_in_danger(E8, W, board) && !square_in_danger(D8, W, board)) {
                    add_nonkill_move(board, create_move_int(E8, C8, NULL_LOCATION, NULL_LOCATION, is_castling()), trackerStructure);
                }
            }
        }

    }

    printf("side: %d\n",turn);
    // Loop for sliding board_squares
    piece_i = SlidingPiecesIndex[turn];
    piece = SlidingPiecesLoop[piece_i];
    piece_i++;

    // comment out for demonstration 4
    while( piece != 0) {
        ASSERT(isPieceValid(piece));
        printf("sliders pceIndex:%d pce:%d\n",piece_i,piece);

        for(int j = 0; j < NumberOfDirections[piece]; ++j) {
            direction = PieceDirectionList[piece][j];
            targetSquare = square + direction;

            while (!is_square_off_board(targetSquare)) { // will continue to loop and move diagonally until out of bounds
                if (board->board_squares[targetSquare] != NULL_LOCATION) { // if we end up with a non empty square
                    if (piece_colour[board->board_squares[targetSquare]] == (turn ^ 1)) { //to affect opposite turn;
                        printf("\t\tDanger at %s\n", print_square(targetSquare));
                    }
                    break;
                }
                printf("\t\tNormal environment surrounding %s\n", print_square(targetSquare));
                targetSquare += direction;
            }
        }

        piece = SlidingPiecesLoop[piece_i];
        piece_i++;
    }

    // comment out for demonstration 4

    while(piece != 0) {
        ASSERT(is_piece_valid(piece));

        for(int k = 0; k < board->piece_number[piece]; ++k) {
            square = board->piece_list[piece][k];
            ASSERT(is_square_on_board(square));

            for(int i = 0; i < NumberOfDirections[piece]; ++i) {
                direction = PieceDirectionList[piece][i];
                targetSquare = square + direction;

                while(!is_square_off_board(targetSquare)) {
                    // B ^ 1 == W       W ^ 1 == B
                    if(board->board_squares[targetSquare] != NULL_LOCATION) {
                        if( piece_colour[board->board_squares[targetSquare]] == (turn ^ 1)) {
                            add_kill_move(board,
                                          create_move_int(square, targetSquare, board->board_squares[targetSquare],
                                                          NULL_LOCATION, 0), trackerStructure);
                        }
                        break;
                    }
                    add_nonkill_move(board, create_move_int(square, targetSquare, NULL_LOCATION, NULL_LOCATION, 0), trackerStructure);
                    targetSquare += direction;
                }
            }
        }

        piece = SlidingPiecesLoop[piece_i++];
    }

    // Loop for non sliding board_squares
    piece_i = NonSlidingPiecesIndex[turn];
    piece = NonSlidingPiecesLoop[piece_i];
    piece_i++;

    while(piece != 0) {
        ASSERT(is_piece_valid(piece));

        for (int i = 0; i < board->piece_number[piece]; ++i) {
            square = board->piece_list[piece][i];
            ASSERT(is_square_on_board(square));

            for(int j = 0; j < NumberOfDirections[piece]; ++j) {
                direction = PieceDirectionList[piece][j];
                targetSquare = square + direction;
                if (is_square_off_board(targetSquare))
                    continue;
                if (board->board_squares[targetSquare] != NULL_LOCATION) {
                    if( piece_colour[board->board_squares[targetSquare]] == (turn^1)) { //to affect opposite turn;
                        // B^1 = W, W^1 = B
                        add_kill_move(board,
                                      create_move_int(square, targetSquare, board->board_squares[targetSquare], NULL_LOCATION,
                                                      0), trackerStructure);
                    }
                    continue;
                }
                add_nonkill_move(board, create_move_int(square, targetSquare, NULL_LOCATION, NULL_LOCATION, 0), trackerStructure);
            }
        }

        piece = NonSlidingPiecesLoop[piece_i];
        piece_i++;
    }
}

void generate_all_moves(const BOARD_STRUCTURE *board, MOVES_TRACKERS_STRUCTURE *list) {
    ASSERT(CheckBoard(board));
    list->count = 0;

    int pce = NULL_LOCATION;
    int turn = board->turn;
    int square = 0; int t_sq = 0;
    int i = 0;
    int dir = 0;
    int index = 0;
    int index_of_piece = 0;

    printf("\n\nColour:%d\n",turn);

    if(turn == W) {

        for(i = 0; i < board->piece_number[whitePawn]; ++i) {
            square = board->piece_list[whitePawn][i];
            ASSERT(SqOnBoard(square));

            if(board->board_squares[square + 10] == NULL_LOCATION) {
                add_wP_nonkill_move(board, square, square+10, list);
                if(BoardRows[square] == ROW_2 && board->board_squares[square + 20] == NULL_LOCATION) {
                    add_nonkill_move(board, create_move_int(square,(square+20),NULL_LOCATION,NULL_LOCATION, is_pawn_start()),list);
                }
            }

            if(!is_square_off_board(square + PAWN_DIAGONAL_ATTACK_2) && piece_colour[board->board_squares[square + PAWN_DIAGONAL_ATTACK_2]] == B) {
                add_wP_kill_move(board, square, square+PAWN_DIAGONAL_ATTACK_2, board->board_squares[square + PAWN_DIAGONAL_ATTACK_2], list);
            }


            if(!is_square_off_board(square + PAWN_DIAGONAL_ATTACK_1) && piece_colour[board->board_squares[square + PAWN_DIAGONAL_ATTACK_1]] == B) {
                add_wP_kill_move(board, square, square+PAWN_DIAGONAL_ATTACK_1, board->board_squares[square + PAWN_DIAGONAL_ATTACK_1], list);
            }

            if(square + PAWN_DIAGONAL_ATTACK_2 == board->enPassant) {
                add_kill_move(board, create_move_int(square,square + PAWN_DIAGONAL_ATTACK_2,NULL_LOCATION,NULL_LOCATION,EN_PASSANT_FLAG), list);
            }
            if(square + PAWN_DIAGONAL_ATTACK_1 == board->enPassant) {
                add_kill_move(board, create_move_int(square,square + PAWN_DIAGONAL_ATTACK_1,NULL_LOCATION,NULL_LOCATION,EN_PASSANT_FLAG), list);
            }
        }
    } else {

        for(i = 0; i < board->piece_number[blackPawn]; ++i) {
            square = board->piece_list[blackPawn][i];
            ASSERT(SqOnBoard(square));
            if(board->board_squares[square - 10] == NULL_LOCATION) {
                add_bP_nonkill_move(board, square, square-10, list);
                if(BoardRows[square] == ROW_7 && board->board_squares[square - 20] == NULL_LOCATION) {
                    add_nonkill_move(board, create_move_int(square,(square-20),NULL_LOCATION,NULL_LOCATION, is_pawn_start()),list);
                }
            }
            if(!is_square_off_board(square - PAWN_DIAGONAL_ATTACK_2) && piece_colour[board->board_squares[square - 9]] == W) {
                add_bP_kill_move(board, square, square-PAWN_DIAGONAL_ATTACK_2, board->board_squares[square - PAWN_DIAGONAL_ATTACK_2], list);
            }
            if(!is_square_off_board(square - PAWN_DIAGONAL_ATTACK_1) && piece_colour[board->board_squares[square - PAWN_DIAGONAL_ATTACK_1]] == W) {
                add_bP_kill_move(board, square, square-PAWN_DIAGONAL_ATTACK_1, board->board_squares[square - 1], list);
            }
            if(square - PAWN_DIAGONAL_ATTACK_2 == board->enPassant) {
                add_kill_move(board, create_move_int(square,square - PAWN_DIAGONAL_ATTACK_2,NULL_LOCATION,NULL_LOCATION,EN_PASSANT_FLAG), list);
            }
            if(square - PAWN_DIAGONAL_ATTACK_1 == board->enPassant) {
                add_kill_move(board, create_move_int(square,square - PAWN_DIAGONAL_ATTACK_1,NULL_LOCATION,NULL_LOCATION,EN_PASSANT_FLAG), list);
            }
        }
    }

    /* Loop for slide board_squares */
    index_of_piece = SlidingPiecesIndex[turn];
    pce = SlidingPiecesLoop[index_of_piece++];
    while( pce != 0) {
        ASSERT(PieceValid(pce));
        printf("index of sliding piece:%d piece:%d\n",index_of_piece,pce);
        pce = SlidingPiecesLoop[index_of_piece++];
    }

    /* Loop for non slide */
    index_of_piece = NonSlidingPiecesIndex[turn];
    pce = NonSlidingPiecesLoop[index_of_piece++];

    while( pce != 0) {
        ASSERT(PieceValid(pce));
        printf("index of non sliding piece:%d piece:%d\n",index_of_piece,pce);

        for(i = 0; i < board->piece_number[pce]; ++i) {
            square = board->piece_list[pce][i];
            ASSERT(SqOnBoard(square));
            printf("Piece:%c on %s\n",pieceCharacters[pce],print_square(square));

            for(index = 0; index < NumberOfDirections[pce]; ++index) {
                dir = PieceDirectionList[pce][index];
                t_sq = square + dir;

                if(is_square_off_board(t_sq)) {
                    continue;
                }

                // B ^ 1 == W       W ^ 1 == B
                if(board->board_squares[t_sq] != NULL_LOCATION) {
                    if((piece_colour[board->board_squares[t_sq]] == turn) ^ 1) {
                        printf("\t\tPiece can make an attacking move to %s\n",print_square(t_sq));
                    }
                    continue;
                }
                printf("\t\tPiece can make an normal move to %s\n",print_square(t_sq));
            }
        }

        pce = NonSlidingPiecesLoop[index_of_piece++];
    }
}


void generate_all_attacks(const BOARD_STRUCTURE *board, MOVES_TRACKERS_STRUCTURE *trackerStructure) {
    ASSERT(run_board_check(board));
    int piece = NULL_LOCATION;
    int turn = board->turn;
    int square = 0;
    int targetSquare = 0;
    int direction = 0;
    int piece_i = 0;
    if (turn == W) { // we want to loop through each one of our white pawns on the board

        for (int i = 0; i < board->piece_number[whitePawn]; ++i) {
            square = board->piece_list[whitePawn][i];
            ASSERT(is_square_on_board(square)) // ensure square that it is giving us is on the board
            if (!is_square_off_board(square + PAWN_DIAGONAL_ATTACK_2) && piece_colour[board->board_squares[square + PAWN_DIAGONAL_ATTACK_2]] == B) {
                add_wP_kill_move(board, square, square + PAWN_DIAGONAL_ATTACK_2, board->board_squares[square + PAWN_DIAGONAL_ATTACK_2], trackerStructure);
            }
            if (!is_square_off_board(square + PAWN_DIAGONAL_ATTACK_1) && piece_colour[board->board_squares[square + PAWN_DIAGONAL_ATTACK_1]] == B) {
                add_wP_kill_move(board, square, square + PAWN_DIAGONAL_ATTACK_1, board->board_squares[square + PAWN_DIAGONAL_ATTACK_1], trackerStructure);
            }

            if (board->enPassant != NO_SQUARE) {
                if (square + PAWN_DIAGONAL_ATTACK_2 == board->enPassant) {
                    add_enpassant_move(board, create_move_int(square, square + PAWN_DIAGONAL_ATTACK_2, NULL_LOCATION, NULL_LOCATION,
                                                              EN_PASSANT_FLAG), trackerStructure);
                }
                if (square + PAWN_DIAGONAL_ATTACK_1 == board->enPassant) {
                    add_enpassant_move(board, create_move_int(square, square + PAWN_DIAGONAL_ATTACK_1, NULL_LOCATION, NULL_LOCATION,
                                                              EN_PASSANT_FLAG), trackerStructure);
                }
            }

        }

    } else {

        for(int i = 0; i < board->piece_number[blackPawn]; ++i) {
            square = board->piece_list[blackPawn][i];
            ASSERT(is_square_on_board(square));

            // black attacking white
            if(!is_square_off_board(square - PAWN_DIAGONAL_ATTACK_2) && piece_colour[board->board_squares[square - PAWN_DIAGONAL_ATTACK_2]] == W) {
                add_bP_kill_move(board, square, square-PAWN_DIAGONAL_ATTACK_2, board->board_squares[square - PAWN_DIAGONAL_ATTACK_2], trackerStructure);
            }
            if(!is_square_off_board(square - PAWN_DIAGONAL_ATTACK_1) && piece_colour[board->board_squares[square - PAWN_DIAGONAL_ATTACK_1]] == W) {
                add_bP_kill_move(board, square, square-PAWN_DIAGONAL_ATTACK_1, board->board_squares[square - PAWN_DIAGONAL_ATTACK_1], trackerStructure);
            }
            if (board->enPassant != NO_SQUARE) {
                if (square - PAWN_DIAGONAL_ATTACK_2 == board->enPassant) {
                    add_enpassant_move(board,
                                       create_move_int(square, square - PAWN_DIAGONAL_ATTACK_2, NULL_LOCATION, NULL_LOCATION,
                                                       EN_PASSANT_FLAG), trackerStructure);
                }
                if (square - PAWN_DIAGONAL_ATTACK_1 == board->enPassant) {
                    add_enpassant_move(board,
                                       create_move_int(square, square - PAWN_DIAGONAL_ATTACK_1, NULL_LOCATION, NULL_LOCATION,
                                                       EN_PASSANT_FLAG), trackerStructure);
                }
            }
        }

    }

    printf("side: %d\n",turn);
    // Loop for sliding board_squares
    piece_i = SlidingPiecesIndex[turn];
    piece = SlidingPiecesLoop[piece_i];
    piece_i++;

//    while( piece != 0) {
//        ASSERT(isPieceValid(piece));
//        printf("sliders pceIndex:%d pce:%d\n",piece_i,piece);
//
//        for(int j = 0; j < NumberOfDirections[piece]; ++j) {
//            direction = PieceDirectionList[piece][j];
//            targetSquare = square + direction;
//
//            while (!isSquareOffBoard(targetSquare)) { // will continue to loop and move diagonally until out of bounds
//                if (board->board_squares[targetSquare] != NULL_LOCATION) { // if we end up with a non empty square
//                    if (piece_colour[board->board_squares[targetSquare]] == (turn ^ 1)) { //to affect opposite turn;
//                        printf("\t\tDanger at %s\n", printSquare(targetSquare));
//                    }
//                    break;
//                }
//                printf("\t\tNormal environment surrounding %s\n", printSquare(targetSquare));
//                targetSquare += direction;
//            }
//        }
//
//        piece = SlidingPiecesLoop[piece_i];
//        piece_i++;
//    }

    while( piece != 0) {
        ASSERT(is_piece_valid(piece));

        for(int k = 0; k < board->piece_number[piece]; ++k) {
            square = board->piece_list[piece][k];
            ASSERT(is_square_on_board(square));

            for(int i = 0; i < NumberOfDirections[piece]; ++i) {
                direction = PieceDirectionList[piece][i];
                targetSquare = square + direction;

                while(!is_square_off_board(targetSquare)) {
                    // B ^ 1 == W       W ^ 1 == B
                    if(board->board_squares[targetSquare] != NULL_LOCATION) {
                        if( piece_colour[board->board_squares[targetSquare]] == (turn ^ 1)) {
                            add_kill_move(board,
                                          create_move_int(square, targetSquare, board->board_squares[targetSquare],
                                                          NULL_LOCATION, 0), trackerStructure);
                        }
                        break;
                    }
                    targetSquare += direction;
                }
            }
        }

        piece = SlidingPiecesLoop[piece_i++];
    }

    // Loop for non sliding board_squares
    piece_i = NonSlidingPiecesIndex[turn];
    piece = NonSlidingPiecesLoop[piece_i];
    piece_i++;

    while(piece != 0) {
        ASSERT(is_piece_valid(piece));

        for (int i = 0; i < board->piece_number[piece]; ++i) {
            square = board->piece_list[piece][i];
            ASSERT(is_square_on_board(square));

            for(int j = 0; j < NumberOfDirections[piece]; ++j) {
                direction = PieceDirectionList[piece][j];
                targetSquare = square + direction;
                if (is_square_off_board(targetSquare))
                    continue;
                if (board->board_squares[targetSquare] != NULL_LOCATION) {
                    if( piece_colour[board->board_squares[targetSquare]] == (turn^1)) { //to affect opposite turn;
                        // B^1 = W, W^1 = B
                        add_kill_move(board,
                                      create_move_int(square, targetSquare, board->board_squares[targetSquare], NULL_LOCATION,
                                                      0), trackerStructure);
                    }
                    continue;
                }
            }
        }

        piece = NonSlidingPiecesLoop[piece_i];
        piece_i++;
    }
}