
#include "search.h"

//Find move repetition
//Pawns cannot moved backwards neither can captured pieces come back
//Therefore iteration can be optimised from the last time fifty move counter was set to zero
int repetition_occured(const BOARD_STRUCTURE *board) {
    for (int i = board->halfMovesPlayedGame - board->fiftyMoveCounter; i < board->halfMovesPlayedGame - 1; ++i) {
        ASSERT(i >= 0 && i < MAX_GAME_MOVES);
        if (board->position_key == board->game_history[i].positionKey) {
            return T;
        }
    }
    return F;
}

static void checkTimeUpOrInterrupt(SEARCH_INFO_STRUCTURE *information) {
    if(information->time_set == T && getElapsedTimeMilliseconds() > information->endTime) {
        information->ended = T;
    }
}

static void choose_next_board(int moveNum, MOVES_TRACKERS_STRUCTURE *movesTrackers) {
    MOVE_STRUCTURE t;
    int bestScore = 0;
    int bestNum = moveNum;

    for (int i = moveNum; i < movesTrackers->count; ++i) {
        if (movesTrackers->moveTracker[i].score > bestScore) {
            bestScore = movesTrackers->moveTracker[i].score;
            bestNum = i;
        }
    }
    t = movesTrackers->moveTracker[moveNum];
    movesTrackers->moveTracker[moveNum] = movesTrackers->moveTracker[bestNum];
    movesTrackers->moveTracker[bestNum] = t;
}

//Principle Variation - THE BEST LINE FOUND BY THE PROGRAM TO THE GIVEN SEARCH DEPTH
const int PV_Size = 0x100000 * NUM_COLOURS;
void load_PV_table(PVTABLE_STRUCTURE *table) {
    table->numEntries = PV_Size / sizeof(PVENTRY_STRUCTURE);
    table->numEntries -= NUM_COLOURS;
    if(table->pTable != NULL) {
        free(table->pTable);
    }
    table->pTable = (PVENTRY_STRUCTURE *) malloc(table->numEntries * sizeof(PVENTRY_STRUCTURE));
    emptyPvTable(table);
    printf("PvTable initialisation finished with %d entries\n",table->numEntries);
}

void emptyPvTable(PVTABLE_STRUCTURE *table) {
    PVENTRY_STRUCTURE *pvEntry;
    for (pvEntry = table->pTable; pvEntry < table->pTable + table->numEntries; ++pvEntry) {
        pvEntry->positionKey = 0ULL;
        pvEntry->move = NO_MOVE;
    }
}

void add_PV_move(const BOARD_STRUCTURE *board, const int move) {
    int index = board->position_key % board->pvTable->numEntries;
    ASSERT(index >= 0 && index <= board->pvTable->numEntries - 1);
    board->pvTable->pTable[index].move = move;
    board->pvTable->pTable[index].positionKey = board->position_key;
}

int probe_PV_table(const BOARD_STRUCTURE *board) {
    int i = board->position_key % board->pvTable->numEntries;
    ASSERT(index >= 0 && index <= board->pvTable->numEntries - 1);
    if(board->pvTable->pTable[i].positionKey == board->position_key ) {
        return board->pvTable->pTable[i].move;
    }
    return NO_MOVE;
}

int PV_line(const int depth, BOARD_STRUCTURE *board) {
    ASSERT(depth < MAXIMUM_DEPTH);
    // first we want to probe the pvTable
    int move = probe_PV_table(board);
    int i = 0;
    while(move != NO_MOVE && i < depth) {
        ASSERT(i < MAXIMUM_DEPTH);
        if (move_exists(board, move)) {
            make_move(board, move);
            board->PV_list[i++] = move;
        } else {
            break;
        }
        move = probe_PV_table(board);
    }
    // we need to take back the moves in the positions
    while(board->halfMovesPlayedCurrent > 0) {
        take_back_move(board);
    }
    return i;
}

static void clearForSearch(BOARD_STRUCTURE *board, SEARCH_INFO_STRUCTURE *information) {
    for(int i = 0; i < NUM_OF_PIECES; ++i) {
        for(int j = 0; j < BOARD_SIZE; ++j) {
            board->search_previous_values[i][j]=0;
        }
    }
    for(int i = 0; i < NUM_COLOURS; ++i) {
        for(int j = 0; j < MAXIMUM_DEPTH; ++j) {
            board->search_killers[i][j]=0;
        }
    }
    emptyPvTable(board->pvTable);
    board->halfMovesPlayedCurrent = 0;
    information->initTime = getElapsedTimeMilliseconds();
    information->ended = 0;
    information->positionsVisited = 0;
    information->failHigh = 0;
    information->failHighFirst = 0;
}

static int quiescence_search(int a, int b, BOARD_STRUCTURE *board, SEARCH_INFO_STRUCTURE *information) {
    ASSERT(CheckBoard(board));
    if ((information->positionsVisited & (MAX_GAME_MOVES-1)) == 0) {
        checkTimeUpOrInterrupt(information);
    }
    information->positionsVisited++;
    if(repetition_occured(board) || board->fiftyMoveCounter >= 100) {
        return 0;
    }
    // are we going too deep?
    if(board->halfMovesPlayedCurrent > MAXIMUM_DEPTH - 1) {
        return evaluate_pos(board);
    }
    int evalScore = evaluate_pos(board);
    if(evalScore >= b) {
        return b;
    }
    if(evalScore > a) {
        a = evalScore;
    }
    MOVES_TRACKERS_STRUCTURE list[1];
    generate_all_attacks(board,list);
    int move_number = 0;
    int legalMoves = 0;
    int previousAlpha = a;
    int best_move = NO_MOVE;
    evalScore = -INFINITY;
    int PvMove = probe_PV_table(board);
    for(move_number = 0; move_number < list->count; ++move_number) {
        choose_next_board(move_number, list);
        if ( !make_move(board, list->moveTracker[move_number].move))  {
            continue;
        }
        legalMoves++;
        evalScore = -quiescence_search(-b, -a, board, information);
        take_back_move(board);
        if(information->ended==T) {
            return 0;
        }
        if(evalScore > a) {
            if(evalScore >= b) {
                if(legalMoves==1) {
                    information->failHighFirst++;
                }
                information->failHigh++;
                return b;
            }
            a = evalScore;
            best_move = list->moveTracker[move_number].move;
        }
    }
    if(a != previousAlpha) {
        add_PV_move(board, best_move);
    }
    return a;
}

static int alpha_beta_search(int a,int b,int depth,BOARD_STRUCTURE *board,SEARCH_INFO_STRUCTURE *info,int nullMove) {
    ASSERT(run_board_check(board));
    if(depth == 0) {
        info->positionsVisited++;
        return quiescence_search(a, b, board, info);
    }

    if ((info->positionsVisited & (MAX_GAME_MOVES-1)) == 0) {
        checkTimeUpOrInterrupt(info);
    }

    info->positionsVisited++;
    if(repetition_occured(board) || board->fiftyMoveCounter >= 100) {
        return 0;
    }
    if(board->halfMovesPlayedCurrent > MAXIMUM_DEPTH - 1) {
        return evaluate_pos(board);
    }
    MOVES_TRACKERS_STRUCTURE trackerStructure[1];
    generate_all_moves(board, trackerStructure);
    int num_legal_moves = 0;
    int previous_alpha = a;
    int best_move = NO_MOVE;
    int evaluation_score = -INFINITY;
    int PVmove = probe_PV_table(board);

    if ( PVmove != NO_MOVE) { // we have a principle variation move
        for (int i = 0; i < trackerStructure->count; ++i) {
            if (trackerStructure->moveTracker[i].move == PVmove) {
                trackerStructure->moveTracker[i].score = 2000000;
                break;
            }
        }
    }


    for(int i = 0; i < trackerStructure->count; ++i) {
        choose_next_board(i, trackerStructure);
        if ( !make_move(board, trackerStructure->moveTracker[i].move))  {
            continue;
        }
        num_legal_moves++;
        evaluation_score = -alpha_beta_search(-b, -a, depth - 1, board, info, T);
        take_back_move(board);
        if(info->ended==T) {
            return 0;
        }
        if(evaluation_score > a) {
            if(evaluation_score >= b) {
                if (num_legal_moves==1) {
                    // increment FHF as we searched the best move first
                    info->failHighFirst++;
                }
                // otherwise just increment FH
                info->failHigh++;
                if (!(trackerStructure->moveTracker[i].move & CAPTURING_MOVE_FLAG)) {
                    board->search_killers[1][board->halfMovesPlayedCurrent] = board->search_killers[0][board->halfMovesPlayedCurrent];
                    board->search_killers[0][board->halfMovesPlayedCurrent] = trackerStructure->moveTracker[i].move;
                }
                return b; // returning beta
            }
            a = evaluation_score;
            best_move = trackerStructure->moveTracker[i].move;
            if (!(trackerStructure->moveTracker[i].move & CAPTURING_MOVE_FLAG)) {
                board->search_previous_values[board->board_squares[from_square(best_move)]][to_square(best_move)] += depth;
            }
        }
    }
    if(num_legal_moves == 0) {
        if(square_in_danger(board->kingSquare[board->turn],board->turn^1,board)) {
            return -MATE + board->halfMovesPlayedCurrent;
        } else {
            return 0;
        }
    }
    if(a != previous_alpha) {
        add_PV_move(board, best_move);
    }
    return a; // returning alpha
}

/* implements iterative deepening (for each depth, do an alpha beta search) e.g.
 * for (depth = 1 to maxDepth)
 *     search using AlphaBeta
 *     next depth              */
void search_position(BOARD_STRUCTURE *board, SEARCH_INFO_STRUCTURE *information) {
    int best_calculated_move = NO_MOVE;
    int best_score = -INFINITY;
    int depth_value = 0;
    int PV_moves = 0;

    clearForSearch(board,information);

    // iterative deepening
    for(int i = 1; i <= information->depth; ++i) {
        best_score = alpha_beta_search(-INFINITY, INFINITY, i, board, information, T);

        if (information->ended == T) {
            break;
        }

        //getting our principal variation, best move in current position
        PV_moves = PV_line(i, board);
        best_calculated_move = board->PV_list[0];
        // in UCI format e.g. info score cp 13  depth 1 nodes 13 time 15 pv f1b5
        printf("info score cp %d depth %d nodes %ld time %d ",
                i,best_score,information->positionsVisited,getElapsedTimeMilliseconds()-information->initTime);

        PV_moves = PV_line(i, board);
        printf("pv");
        for(int j = 0; j < PV_moves; ++j) {
            printf(" %s", print_move(board->PV_list[j]));
        }
        printf("\n");
        printf("Ordering: %.2f\n", (information->failHighFirst / information->failHigh));
    }
    printf("Best calculated move found: %s\n", print_move(best_calculated_move));
}

