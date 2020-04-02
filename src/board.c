#include "board.h"

void update_piece_values_lists(BOARD_STRUCTURE *board) {
    int piece;
    int square;
    int colour;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        square = i;
        piece = board->board_squares[i];
        if (piece != OUT_OF_BOUNDS && piece != NULL_LOCATION) {
            colour = piece_colour[piece];
            // Setting our piece list up, piece counters up, materials list up and the king squares
            if (non_pawn_pieces[piece] == T) {
                board->nonPawnPieces[colour]++;
            }
            if (major_pieces[piece] == T) {
                board->major_board_pieces[colour]++;
            }
            if (minor_pieces[piece] == T) {
                board->minor_board_pieces[colour]++;
            }
            board->material_value[colour] += piece_value[piece];
            ASSERT(board->piece_number[piece] < 10 && board->piece_number[piece] >= 0);
            board->piece_list[piece][board->piece_number[piece]] = square;
            board->piece_number[piece]++;
            if (piece == whiteKing) {
                board->kingSquare[W] = square;
            }
            if (piece == blackKing) {
                board->kingSquare[B] = square;
            }
            if (piece == whitePawn) {
                set_bit(&board->pawns[W], Base120ToBase64[square]);
                set_bit(&board->pawns[B_AND_W], Base120ToBase64[square]);
            } else if (piece == blackPawn) {
                set_bit(&board->pawns[B], Base120ToBase64[square]);
                set_bit(&board->pawns[B_AND_W], Base120ToBase64[square]);
            }
        }
    }
}

void reset_board(BOARD_STRUCTURE *board) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        //everything is set as out of bounds
        board->board_squares[i] = OUT_OF_BOUNDS;
    }
    for (int j = 0; j < ACTUAL_BOARD_SIZE; ++j) {
        //sets the actual board (not the borders) as empty instead of out of bounds
        board->board_squares[Base64ToBase120[j]] = NULL_LOCATION;
    }
    for (int k = 0; k < NUM_COLOURS; ++k) {
        board->nonPawnPieces[k] = 0;
        board->material_value[k] = 0;
        board->major_board_pieces[k] = 0;
        board->minor_board_pieces[k] = 0;
    }
    for (int l = 0; l < NUM_COLOURS + 1; ++l) {
        board->pawns[l] = 0ULL;
    }
    //resetting the piece number by piece type to zero
    for (int m = 0; m < NUM_OF_PIECES; ++m) {
        board->piece_number[m] = 0;
    }
    board->kingSquare[W] = board->kingSquare[B] = NO_SQUARE;
    board->turn = B_AND_W;
    board->enPassant = NO_SQUARE;
    board->fiftyMoveCounter = NULL_MOVE;
    board->halfMovesPlayedCurrent = NULL_MOVE; //NUM OF HALF MOVES PLAYED IN THE CURRENT SEARCH
    board->halfMovesPlayedGame = NULL_MOVE; //NUM OF HISTORY HALF MOVES PLAYED IN THE WHOLE GAME
    board->castling_permission = NULL_MOVE;
    board->position_key = 0ULL;
}

int translate_FEN(char *fen_string, BOARD_STRUCTURE *board) {
    ASSERT(fen_string != NULL);
    ASSERT(board != NULL);
    reset_board(board);
    int row = ROW_8;
    int column = COLUMN_A;
    int boardPiece = NULL_MOVE;
    int emptySquares = NULL_MOVE;
    int square_64 = NULL_MOVE;
    int square_120 = NULL_MOVE;

    /* First part of FEN is parsed */
    while ((row >= ROW_1) && *fen_string) {
        emptySquares = 1;
        switch (*fen_string) {
            case 'p':
                boardPiece = blackPawn;
                break;
            case 'r':
                boardPiece = blackRook;
                break;
            case 'n':
                boardPiece = blackKnight;
                break;
            case 'b':
                boardPiece = blackBishop;
                break;
            case 'k':
                boardPiece = blackKing;
                break;
            case 'q':
                boardPiece = blackQueen;
                break;
            case 'P':
                boardPiece = whitePawn;
                break;
            case 'R':
                boardPiece = whiteRook;
                break;
            case 'N':
                boardPiece = whiteKnight;
                break;
            case 'B':
                boardPiece = whiteBishop;
                break;
            case 'K':
                boardPiece = whiteKing;
                break;
            case 'Q':
                boardPiece = whiteQueen;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                boardPiece = NULL_LOCATION;
                emptySquares = *fen_string - '0';
                break;
            case '/':
            case ' ': // means we are at the end of the pieces in the FEN translation, or we have a new rank
                row--;
                column = COLUMN_A;
                fen_string++;
                continue;

            default:
                printf("Error during FEN translation \n");
                return -1;
        }

        for (int i = 0; i < emptySquares; i++) {
            square_64 = row * TWO_POWER_THREE + column;
            square_120 = Base64ToBase120[square_64];
            if (boardPiece != NULL_LOCATION) {
                board->board_squares[square_120] = boardPiece;
            }
            column++;
            }
        fen_string++;
        /* fen_string pointer should now be pointing at the 'w' or 'b' in the FEN translation (after the board positions)
         * e.g. rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
         *                                                  ^ *fen_string should be pointing here */
    }

    ASSERT(*fen_string=='b'||*fen_string=='w');
    if (*fen_string=='w') {
        board->turn = W;
    } else {
        board->turn = B;
    }

    fen_string += FEN_JUMP_COLOUR_CASTLING;

    // Setting up castling permission, castling permission takes four characters

    for (int x = 0; x < 4; x++) {
        if (*fen_string == ' ')
            break;
        switch(*fen_string) {
            case 'K':
                board->castling_permission |= WHITEKINGCASTLING;
                break;
            case 'Q':
                board->castling_permission |= WHITEQUEENCASTLING;
                break;
            case 'k':
                board->castling_permission |= BLACKKINGCASTLING;
                break;
            case 'q':
                board->castling_permission |= BLACKQUEENCASTLING;
                break;
            default:
                break;
        }
        fen_string++;
    }
    fen_string++; // incrementing from here will move on to the En Passant square

    // Dealing with the En Passant square

    ASSERT(board->castling_permission>=0 && board->castling_permission<=15);
    // we have an en passant square if this statement holds
    if (*fen_string != '-') {
        column = fen_string[0] - 'a';
        row = fen_string[1] - '1';

        //checking if they're on the board
        ASSERT(column>=COLUMN_A && column <= COLUMN_H);
        ASSERT(row >= ROW_1 && row <= ROW_8);
        board->enPassant = convert_64_to_120(column, row);
    }
    board->position_key = initialise_position_key(board);
    update_piece_values_lists(board);
    return 0;
}

void print_board(const BOARD_STRUCTURE *board) {
    int piece;
    printf("\nZONA GAME BOARD\n");
    for (int row = ROW_8; row >= ROW_1; --row) {
        printf("%d ", row + 1);
        for (int column = COLUMN_A; column <= COLUMN_H; ++column) {
            piece = board->board_squares[convert_64_to_120(column, row)];
            printf("%3c", pieceCharacters[piece]);
        }
        printf("\n");
    }
    printf("  ");
    for (int column = COLUMN_A; column <= COLUMN_H; ++column) {
        printf("%3c", 'a' + column);
    }
    printf("\n");
    printf("Turn: %c\n", turn_characters[board->turn]);
    printf("En Passant: %d\n", board->enPassant);
    printf("Castling: %c%c%c%c\n",
            board->castling_permission&WHITEKINGCASTLING  ?  'K'  : '-',
            board->castling_permission&WHITEQUEENCASTLING ?  'Q'  : '-',
            board->castling_permission&BLACKKINGCASTLING  ?  'k'  : '-',
            board->castling_permission&BLACKQUEENCASTLING ?  'q'  : '-'
            );
    printf("Hash Key: %llX\n", board->position_key);
}

int run_board_check(const BOARD_STRUCTURE *board) {
    //temporary variables
    U64_t temporary_pawn[3] = {0ULL, 0ULL, 0ULL};
    int colour;
    int materials_temporary[2] = { 0, 0};
    int p;
    int square_120;
    int t;
    int temporary_major_pieces[2] = { 0, 0};
    int temporary_minor_pieces[2] = { 0, 0};
    int temporary_non_pawn_pieces[2] = { 0, 0};
    int temporary_piece_number[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    temporary_pawn[B] = board->pawns[B];
    temporary_pawn[B_AND_W] = board->pawns[B_AND_W];
    temporary_pawn[W] = board->pawns[W];

    for(int w = whitePawn; w <= blackKing; ++w) {
        for(int b = 0; b < board->piece_number[w]; ++b) {
            square_120 = board->piece_list[w][b];
            ASSERT(board->board_squares[square_120]==w);
        }
    }

    for(int i = 0; i < ACTUAL_BOARD_SIZE; ++i) {
        square_120 = Base64ToBase120[i];
        t = board->board_squares[square_120];
        temporary_piece_number[t]++;
        colour = piece_colour[t];
        if( non_pawn_pieces[t] == T){
            temporary_non_pawn_pieces[colour]++;
        }
        if( minor_pieces[t] == T) {
            temporary_minor_pieces[colour]++;
        }
        if( major_pieces[t] == T) {
            temporary_major_pieces[colour]++;
        }
        materials_temporary[colour] += piece_value[t];
    }

    for(t = whitePawn; t <= blackKing; ++t) {
        ASSERT(temporary_piece_number[t]==board->piece_number[t]);
    }

    p = get_bits_count(temporary_pawn[W]);
    ASSERT(p == board->piece_number[whitePawn]);
    p = get_bits_count(temporary_pawn[B]);
    ASSERT(p == board->piece_number[blackPawn]);
    p = get_bits_count(temporary_pawn[B_AND_W]);
    ASSERT(p == (board->piece_number[blackPawn] + board->piece_number[whitePawn]));
    int i;

    while(temporary_pawn[W]) {
        i = unset_bit(&temporary_pawn[W]);
        ASSERT(board->board_squares[Base64ToBase120[i]] == whitePawn);
    }
    while(temporary_pawn[B]) {
        i = unset_bit(&temporary_pawn[B]);
        ASSERT(board->board_squares[Base64ToBase120[i]] == blackPawn);
    }
    while(temporary_pawn[B_AND_W]) {
        i = unset_bit(&temporary_pawn[B_AND_W]);
        ASSERT( (board->board_squares[Base64ToBase120[i]] == blackPawn) || (board->board_squares[Base64ToBase120[i]] == whitePawn) );
    }
    ASSERT(materials_temporary[W]==board->material_value[W] && materials_temporary[B]==board->material_value[B]);
    ASSERT(temporary_minor_pieces[W]==board->minor_board_pieces[W] && temporary_minor_pieces[B]==board->minor_board_pieces[B]);
    ASSERT(temporary_major_pieces[W]==board->major_board_pieces[W] && temporary_major_pieces[B]==board->major_board_pieces[B]);
    ASSERT(temporary_non_pawn_pieces[W]==board->nonPawnPieces[W] && temporary_non_pawn_pieces[B]==board->nonPawnPieces[B]);
    ASSERT(board->turn==W || board->turn==B);
    ASSERT(initialise_position_key(board)==board->position_key);
    ASSERT(board->enPassant==NO_SQUARE || ( BoardRows[board->enPassant]==ROW_6 && board->turn == W)
           || ( BoardRows[board->enPassant]==ROW_3 && board->turn == B));
    ASSERT(board->board_squares[board->kingSquare[W]] == whiteKing);
    ASSERT(board->board_squares[board->kingSquare[B]] == blackKing);
    return T;
}

BOARD_STRUCTURE * generate_board() {
    BOARD_STRUCTURE * board = (BOARD_STRUCTURE*)malloc(sizeof(BOARD_STRUCTURE));
    board->pvTable->pTable = NULL;
    return board;
}