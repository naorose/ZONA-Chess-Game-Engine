#include "inputOutput.h"

char *print_square(const int64_t square) {
    static char squareString[NUM_COLOURS + 1];
    int column = BoardColumns[square];
    int row = BoardRows[square];
    sprintf(squareString, "%c%c", ('a' + column), ('1' + row));
    return squareString;
}

char *print_move(const int move_to_print) {
    static char moveString[6];
    int columnf = BoardColumns[from_square(move_to_print)];
    int rowf = BoardRows[from_square(move_to_print)];
    int columnt = BoardColumns[to_square(move_to_print)];
    int rowt = BoardRows[to_square(move_to_print)];
    int piece_to_prom = promotePiece(move_to_print);
    if(piece_to_prom) {
        char promotedChar = 'q';
        if(is_piece_knight[piece_to_prom]) {
            promotedChar = 'n';
        } else if(is_piece_rook_queen[piece_to_prom] && !is_piece_bishop_queen[piece_to_prom])  {
            promotedChar = 'r';
        } else if(!is_piece_rook_queen[piece_to_prom] && is_piece_bishop_queen[piece_to_prom])  {
            promotedChar = 'b';
        }
        sprintf(moveString, "%c%c%c%c%c",('a'+columnf),('1'+rowf),('a'+columnt),('1'+rowt), promotedChar);
    } else {
        sprintf(moveString, "%c%c%c%c",('a'+columnf),('1'+rowf),('a'+columnt),('1'+rowt));
    }
    return moveString;
}

#define STRING_0 0
#define STRING_1 1
#define STRING_2 2
#define STRING_3 3
#define STRING_4 4


int translate_move(char *s, BOARD_STRUCTURE *pos) {
    ASSERT(run_board_check(pos));
    if(s[STRING_1] > '8' || s[STRING_1] < '1') {
        return NO_MOVE;
    }
    if(s[STRING_3] > '8' || s[STRING_3] < '1') {
        return NO_MOVE;
    }
    if(s[STRING_0] > 'h' || s[STRING_0] < 'a') {
        return NO_MOVE;
    }
    if(s[STRING_2] > 'h' || s[STRING_2] < 'a') {
        return NO_MOVE;
    }
    int from = convert_64_to_120(s[STRING_0] - 'a', s[STRING_1] - '1');
    int to = convert_64_to_120(s[STRING_2] - 'a', s[STRING_3] - '1');
    ASSERT(is_square_on_board(from) && is_square_on_board(to));
    MOVES_TRACKERS_STRUCTURE list[1];
    generate_all_moves(pos, list);
    int m = 0;
    int promotedPiece = NULL_LOCATION;
    for(int i = 0; i < list->count; ++i) {
        m = list->moveTracker[i].move;
        if(from_square(m)==from && to_square(m)==to) {
            promotedPiece = promotePiece(m);
            if(promotedPiece!=NULL_LOCATION) {
                if(is_piece_rook_queen[promotedPiece] && !is_piece_bishop_queen[promotedPiece] && s[STRING_4]=='r') {
                    return m;
                } else if(!is_piece_rook_queen[promotedPiece] && is_piece_bishop_queen[promotedPiece] && s[STRING_4]=='b') {
                    return m;
                } else if(is_piece_rook_queen[promotedPiece] && is_piece_bishop_queen[promotedPiece] && s[STRING_4]=='q') {
                    return m;
                } else if(is_piece_knight[promotedPiece]&& s[STRING_4]=='n') {
                    return m;
                }
                continue;
            }
            return m;
        }
    }
    return NO_MOVE;
}

void print_move_tracker_list_no_score(const MOVES_TRACKERS_STRUCTURE *moves) {
    int scoreVal = 0;
    int m = 0;
    printf("Move Tracker List: \n");
    for(int i = 0; i < moves->count; ++i) {
        m = moves->moveTracker[i].move;
        scoreVal = moves->moveTracker[i].score;
        printf("Move: %d > %s\n",i+1, print_move(m));
    }
    printf("Total number of moves: %d\n\n",moves->count); //***
}

void print_move_tracker_list(const MOVES_TRACKERS_STRUCTURE *moves) {
    int scoreVal = 0;
    int move = 0;
    printf("Move Tracker List: \n");
    for(int i = 0; i < moves->count; ++i) {
        move = moves->moveTracker[i].move;
        scoreVal = moves->moveTracker[i].score;
        printf("Move: %d > %s (Score: %d)\n",i+1, print_move(move),scoreVal);
    }
    printf("Total number of moves: %d\n\n",moves->count); //***
}

