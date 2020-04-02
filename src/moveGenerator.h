#ifndef ZONA_CHESS_ENGINE_MOVEGENERATOR_H
#define ZONA_CHESS_ENGINE_MOVEGENERATOR_H


#include "data.h"
#include "structures.h"
#include "attack.h"
#include "assertions.h"
#include "board.h"
#include "inputOutput.h"
#include "moveMaker.h"
#include <stdio.h>

#define NO_MOVES 0
#define FOUR_MOVES 4
#define EIGHT_MOVES 8
int create_move_int(int f, int t, int ca, int pro, int fl);
int is_square_off_board(int square);
int load_MVV_LVA();
static void add_bP_kill_move( const BOARD_STRUCTURE *board, const int from, const int to, const int cap, MOVES_TRACKERS_STRUCTURE *list );
static void add_bP_nonkill_move( const BOARD_STRUCTURE *board, const int from, const int to, MOVES_TRACKERS_STRUCTURE *list );
static void add_enpassant_move(const BOARD_STRUCTURE *board, int move, MOVES_TRACKERS_STRUCTURE *trackerStructure);
static void add_kill_move(const BOARD_STRUCTURE *board, int move, MOVES_TRACKERS_STRUCTURE *trackerStructure);
static void add_nonkill_move(const BOARD_STRUCTURE *board, int move, MOVES_TRACKERS_STRUCTURE *trackerStructure);
static void add_wP_kill_move( const BOARD_STRUCTURE *board, const int from, const int to, const int cap, MOVES_TRACKERS_STRUCTURE *list );
static void add_wP_nonkill_move( const BOARD_STRUCTURE *board, const int from, const int to, MOVES_TRACKERS_STRUCTURE *trackerStructure );
void generate_all_attacks(const BOARD_STRUCTURE *board, MOVES_TRACKERS_STRUCTURE *trackerStructure);
void generate_all_moves(const BOARD_STRUCTURE *board, MOVES_TRACKERS_STRUCTURE *trackerStructure);

#endif //ZONA_CHESS_ENGINE_MOVEGENERATOR_H
