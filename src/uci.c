#include <stdio.h>
#include "uci.h"
#include <string.h>

/*
 *
 * Implementing the UCI protocol was greatly aided through online tutorials and chess wikis,
 * some code is sourced from there
 */

void translate_go_uci(char *input_line, SEARCH_INFO_STRUCTURE *information, BOARD_STRUCTURE *board) {
    int depth_value = NEG, moves_left = 30,move_time = NEG;
    int time_value = NEG, inc = NEUT;
    char *p = NULL;
    information->time_set = F;
    if ((p = strstr(input_line,"infinite"))) {
        ;
    }
    if ((p = strstr(input_line,"binc")) && board->turn == B) {
        inc = atoi(p + 5);
    }
    if ((p = strstr(input_line,"winc")) && board->turn == W) {
        inc = atoi(p + 5);
    }
    if ((p = strstr(input_line,"wtime")) && board->turn == W) {
        time_value = atoi(p + UCI_VALUE6);
    }
    if ((p = strstr(input_line,"btime")) && board->turn == B) {
        time_value = atoi(p + UCI_VALUE6);
    }
    if ((p = strstr(input_line,"movestogo"))) {
        moves_left = atoi(p + UCI_VALUE10);
    }
    if ((p = strstr(input_line,"movetime"))) {
        move_time = atoi(p + UCI_VALUE9);
    }
    if ((p = strstr(input_line,"depth"))) {
        depth_value = atoi(p + UCI_VALUE6);
    }
    if(move_time != NEG) {
        time_value = move_time;
        moves_left = POS;
    }
    information->initTime = getElapsedTimeMilliseconds();
    information->depth = depth_value;
    if(time_value != NEG) {
        information->time_set = T;
        time_value /= moves_left;
        time_value -= 50;
        information->endTime = information->initTime + time_value + inc;
    }
    if(depth_value == NEG) { // depth wasn't set
        information->depth = MAXIMUM_DEPTH;
    }
    // print in UCI format
    printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",time_value,information->initTime,information->endTime,information->depth,information->time_set);
    search_position(board, information);
}
void translate_position_uci(char *input_line, BOARD_STRUCTURE *board) {
    input_line += UCI_VALUE9;
    char *s = input_line;
    if(strncmp(input_line, "startpos", UCI_VALUE8) == NEUT){
        translate_FEN(INIT_FEN, board);
    } else {
        s = strstr(input_line, "fen");
        if(s == NULL) {
            translate_FEN(INIT_FEN, board);
        } else {
            s+=UCI_VALUE4;
            translate_FEN(s, board);
        }
    }
    s = strstr(input_line, "moves");
    int move;
    if(s != NULL) {
        s += UCI_VALUE6;
        while(*s) {
            move = translate_move(s, board);
            if(move == NO_MOVE) break;
            make_move(board, move);
            board->halfMovesPlayedCurrent=NEUT;
            while(*s && *s!= ' ') {
                s++;
            }
            s++; // then we will be at the next move
        }
    }
    print_board(board);
}

void uci_loop() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    char ln[MAX_INPUT_STRING];
    //printing as per the UCI protocol
    printf("id name %s\n",NAME);
    printf("id author x\n");
    printf("uciok\n"); // telling GUI we have started in UCI
    BOARD_STRUCTURE board[POS];
    SEARCH_INFO_STRUCTURE information[POS];
    load_PV_table(board->pvTable);
    while (T) {
        memset(&ln[NEUT], NEUT, sizeof(ln));
        fflush(stdout);
        if (!fgets(ln, MAX_INPUT_STRING, stdin)) {
            continue;
        }
        if (ln[NEUT]=='\n') {
            continue;
        }
        if (!strncmp(ln, "isready", 7)) {
            printf("readyok\n");
            continue;
        } else if (!strncmp(ln, "position", UCI_VALUE8)) {
            translate_position_uci(ln, board);
        } else if (!strncmp(ln, "ucinewgame", UCI_VALUE10)) {
            translate_position_uci("position startpos\n", board);
        } else if (!strncmp(ln, "go", UCI_VALUE2)) {
            translate_go_uci(ln, information, board);
        } else if (!strncmp(ln, "quit", UCI_VALUE4)) {
            information->quit = T;
            break;
        } else if (!strncmp(ln, "uci", UCI_VALUE3)) {
            printf("id name %s\n",NAME);
            printf("id author x\n");
            printf("uciok\n");
        }
        if(information->quit) break;
    }
    free(board->pvTable->pTable);
}