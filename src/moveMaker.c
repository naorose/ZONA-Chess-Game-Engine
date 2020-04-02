
#include "moveMaker.h"


//Mirror of internal board with 120 squares
//Squares used ofr castling do not have value 15
const int CastlePerm[120] = {
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

void hash_board_piece(int piece, int square, BOARD_STRUCTURE *board) {
    board->position_key ^= piece_values[piece][square];
}

void hash_enpassant(BOARD_STRUCTURE *board) {
    board->position_key ^= piece_values[NULL_LOCATION][board->enPassant];
}

void hash_castling_permission(BOARD_STRUCTURE *board) {
    board->position_key ^= castling_values[board->castling_permission];
}

void hashSide(BOARD_STRUCTURE *board) {
    board->position_key ^= side_value;
}

static void remove_from_board(const int square, BOARD_STRUCTURE *board) {
    ASSERT(is_square_on_board(square));
    int boardPiece = board->board_squares[square];
    ASSERT(is_piece_valid(boardPiece));
    int colour = piece_colour[boardPiece];
    int tempPieceNum = -1;
    hash_board_piece(boardPiece, square, board);
    board->board_squares[square] = NULL_LOCATION;
    board->material_value[colour] -= piece_value[boardPiece];
    //Find type of piece
    if (non_pawn_pieces[boardPiece]) {
        board->nonPawnPieces[colour]--;
        if (major_pieces[boardPiece]) {
            board->major_board_pieces[colour]--;
        } else {
            board->minor_board_pieces[colour]--;
        }
    } else {
        clear_bit(&board->pawns[colour], Base120ToBase64[square]);
        clear_bit(&board->pawns[B_AND_W], Base120ToBase64[square]);
    }

    //Remove piece from piece list
    //Loop through the piece index until we find a square equivalent to our square
    for (int i = 0; i < board->piece_number[boardPiece]; ++i) {
        if (board->piece_list[boardPiece][i] == square) {
            tempPieceNum = i;
            break;
        }
    }

    ASSERT(tempPieceNum != -1);
    //Overwrite the removed piece
    board->piece_number[boardPiece]--;
    board->piece_list[boardPiece][tempPieceNum] = board->piece_list[boardPiece][board->piece_number[boardPiece]];
}

static void add_to_board(const int square, BOARD_STRUCTURE *board, const int piece) {
    ASSERT(is_piece_valid(piece));
    ASSERT(is_square_on_board(square));
    int colour = piece_colour[piece];
    hash_board_piece(piece, square, board);
    board->board_squares[square] = piece;
    //Find type of piece
    if (non_pawn_pieces[piece]) {
        board->nonPawnPieces[colour]++;
        if (major_pieces[piece]) {
            board->major_board_pieces[colour]++;
        } else {
            board->minor_board_pieces[colour]++;
        }
    } else {
        set_bit(&board->pawns[colour], Base120ToBase64[square]);
        set_bit(&board->pawns[B_AND_W], Base120ToBase64[square]);
    }
    board->material_value[colour] += piece_value[piece];
    board->piece_list[piece][board->piece_number[piece]] = square;
    board->piece_number[piece]++;
}

static void move_board_piece(const int fromLocation, const int toLocation, BOARD_STRUCTURE *board) {
    ASSERT(is_square_on_board(fromLocation));
    ASSERT(is_square_on_board(toLocation));
    int boardPiece = board->board_squares[fromLocation];
    int colour = piece_colour[boardPiece];
#ifdef DEBUG
    int tempPieceHolder = FALSE;
#endif
    hash_board_piece(boardPiece, fromLocation, board);
    board->board_squares[fromLocation] = NULL_LOCATION;
    hash_board_piece(boardPiece, toLocation, board);
    board->board_squares[toLocation] = boardPiece;
    //Check if piece is a pawn
    if (!non_pawn_pieces[boardPiece]) {
        clear_bit(&board->pawns[colour], Base120ToBase64[fromLocation]);
        clear_bit(&board->pawns[B_AND_W], Base120ToBase64[fromLocation]);
        set_bit(&board->pawns[colour], Base120ToBase64[toLocation]);
        set_bit(&board->pawns[B_AND_W], Base120ToBase64[toLocation]);
    }

    //Cycle through all pieces of the same type and when we find the piece we set its new location
    for (int i = 0; i < board->piece_number[boardPiece]; ++i) {
        if (board->piece_list[boardPiece][i] == fromLocation) {
            board->piece_list[boardPiece][i] = toLocation;
#ifdef DEBUG
            tempPieceHolder = TRUE;
#endif
            break;
        }
    }
    ASSERT(tempPieceHolder);
}

int make_move(BOARD_STRUCTURE *board, int move) {
    ASSERT(run_board_check(board));

    int fromSq = from_square(move);
    int toSq = to_square(move);
    int turn = board->turn;
    ASSERT(is_square_on_board(fromSq));
    ASSERT(is_square_on_board(toSq));
    ASSERT(is_turn_valid(turn));
    ASSERT(is_piece_valid(board->board_squares[fromSq]));
    board->game_history[board->halfMovesPlayedGame].positionKey = board->position_key;
    //Check if move is enPassant
    if (move & EN_PASSANT_FLAG) {
        if (turn == W) {
            remove_from_board(toSq - ENPASSANT_DIST, board);
        } else {
            remove_from_board(toSq + ENPASSANT_DIST, board);
        }
    } else if (move & is_castling()) {
        switch (toSq) {
            case C1:
                move_board_piece(A1, D1, board);
            break;
            case C8:
                move_board_piece(A8, D8, board);
            break;
            case G1:
                move_board_piece(H1, F1, board);
            break;
            case G8:
                move_board_piece(H8, F8, board);
            break;
            default:
                ASSERT(F);
        }
    }
    if (board->enPassant != NO_SQUARE) {
        hash_enpassant(board);
    }
    //Castling permission is hashed out, re calculated and then hashed back in
    hash_castling_permission(board);

    //Game history is also updated
    board->game_history[board->halfMovesPlayedGame].move = move;
    board->game_history[board->halfMovesPlayedGame].fiftyMoveCounter = board->fiftyMoveCounter;
    board->game_history[board->halfMovesPlayedGame].enPassant = board->enPassant;
    board->game_history[board->halfMovesPlayedGame].castlePermission = board->castling_permission;
    board->castling_permission &= CastlePerm[fromSq];
    board->castling_permission &= CastlePerm[toSq];
    board->enPassant = NO_SQUARE;
    hash_castling_permission(board);
    //fifty move rule and captures piece
    //fifty moves without a capture results in a stalemate
    int captured = piece_attacked(move);
    board->fiftyMoveCounter++;
    if (captured != NULL_LOCATION) {
        ASSERT(is_piece_valid(captured));
        remove_from_board(toSq, board);
        board->fiftyMoveCounter = 0;
    }
    //Increment half moves counter
    board->halfMovesPlayedGame++;
    board->halfMovesPlayedCurrent++;
    //Check if we need to set a new EnPassant square
    if (is_piece_pawn[board->board_squares[fromSq]]) {
        board->fiftyMoveCounter = 0;
        if (move & is_pawn_start()) {
            if (turn == W) {
                board->enPassant = fromSq + ENPASSANT_DIST;
                ASSERT(BoardRows[board->enPassant] == ROW_3);
            } else {
                board->enPassant = fromSq - ENPASSANT_DIST;
                ASSERT(BoardRows[board->enPassant] = ROW_6);
            }
            hash_enpassant(board);
        }
    }

    move_board_piece(fromSq, toSq, board);
    //Check if a piece has been promoted
    int promoted = promotePiece(move);
    if (promoted != NULL_LOCATION) {
        ASSERT(is_piece_valid(promoted) && !is_piece_pawn[promoted]);
        remove_from_board(toSq, board);
        add_to_board(toSq, board, promoted);
    }
    if (is_piece_king[board->board_squares[toSq]]) {
        board->kingSquare[turn] = toSq;
    }

    //change side
    board->turn ^= 1;
    hashSide(board);
    ASSERT(run_board_check(board));
    if (square_in_danger(board->kingSquare[turn], board->turn, board)) {
        take_back_move(board);
        return F;
    }
    return T;
}

void take_back_move(BOARD_STRUCTURE *board) {
    //Undo the move that was just implemented
    ASSERT(run_board_check(board));
    board->halfMovesPlayedCurrent--;
    board->halfMovesPlayedGame--;
    //ASSERT(board->halfMovesPlayedGame >= 0 && board->halfMovesPlayedGame < MAX_GAME_MOVES);
    //ASSERT(board->halfMovesPlayedCurrent >= 0 && board->halfMovesPlayedGame < ACTUAL_BOARD_SIZE);
    int move = board->game_history[board->halfMovesPlayedGame].move;
    int from = from_square(move);
    int to = to_square(move);
    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));
    if (board->enPassant != NO_SQUARE) {
        hash_enpassant(board);
    }
    hash_castling_permission(board);
    board->castling_permission = board->game_history[board->halfMovesPlayedGame].castlePermission;
    board->fiftyMoveCounter = board->game_history[board->halfMovesPlayedGame].fiftyMoveCounter;
    board->enPassant = board->game_history[board->halfMovesPlayedGame].enPassant;
    if (board->enPassant != NO_SQUARE) {
        hash_enpassant(board);
    }
    hash_castling_permission(board);
    board->turn ^= 1;
    hashSide(board);
    if (EN_PASSANT_FLAG & move) {
        if(board->turn == W) {
            add_to_board(to - ENPASSANT_DIST, board, blackPawn);
        } else {
            add_to_board(to + ENPASSANT_DIST, board, whitePawn);
        }
    } else if (is_castling() & move) {
        switch(to) {
            case C1:
                move_board_piece(D1, A1, board); break;
            case C8:
                move_board_piece(D8, A8, board); break;
            case G1:
                move_board_piece(F1, H1, board); break;
            case G8:
                move_board_piece(F8, H8, board); break;
            default: ASSERT(F);
        }
    }
    move_board_piece(to, from, board);
    if (is_piece_king[board->board_squares[from]]) {
        board->kingSquare[board->turn] = from;
    }
    int killed = piece_attacked(move);
    if (killed != NULL_LOCATION) {
        ASSERT(is_piece_valid(killed));
        add_to_board(to, board, killed);
    }
    if (promotePiece(move) != NULL_LOCATION){
        ASSERT(is_piece_valid(promotePiece(move)) && !is_piece_pawn[promotePiece(move)]);
        remove_from_board(from, board);
        add_to_board(from, board, (piece_colour[promotePiece(move)] == W ? whitePawn : blackPawn));
    }
    ASSERT(run_board_check(board));
}