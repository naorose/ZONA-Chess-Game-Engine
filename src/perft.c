#include "perft.h"
long leaf_node;
//Recursively implements the perft test
void perft(int depth, BOARD_STRUCTURE *board) {
    ASSERT(run_board_check(board));
    if (depth == 0) {
        leaf_node++;
        return;
    }
    MOVES_TRACKERS_STRUCTURE trackerStructure[1];
    generate_all_moves(board, trackerStructure);
    for (int i = 0; i < trackerStructure->count; ++i) {
        if (!make_move(board, trackerStructure->moveTracker[i].move)) {
            continue;
        }
        perft(depth - 1, board);
        take_back_move(board);
    }
}
void perft_test(int depth, BOARD_STRUCTURE *board) {
    ASSERT(run_board_check(board));
    print_board(board);
    printf("\nTesting Depth: %d\n", depth);
    leaf_node = 0;
    int x = getElapsedTimeMilliseconds();
    MOVES_TRACKERS_STRUCTURE list[1];
    generate_all_moves(board, list);
    int move;
    for (int i = 0; i < list->count; ++i) {
        move = list->moveTracker[i].move;
        if (!make_move(board, move)) {
            continue;
        }
        long cNodes = leaf_node;
        perft(depth - 1, board);
        take_back_move(board);
        long previousNodes = leaf_node - cNodes;
        printf("Move %d : %s : %ld\n", i + 1, print_move(move), previousNodes);
    }
    printf("\nTest Result : %ld leaf nodes have been reached in %dms\n", leaf_node, getElapsedTimeMilliseconds() - x);
}