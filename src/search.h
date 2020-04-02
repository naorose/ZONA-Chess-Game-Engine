#ifndef ZONA_CHESS_ENGINE_SEARCH_H
#define ZONA_CHESS_ENGINE_SEARCH_H

#include <stdio.h>
#include "structures.h"
#include "moveMaker.h"
#include "inputOutput.h"
#include "eval.h"
#include "miscellaneous.h"
#define INFINITY 30000
#define MATE 29000

int PV_line(const int depth, BOARD_STRUCTURE *board);
int probe_PV_table(const BOARD_STRUCTURE *board);
int repetition_occured(const BOARD_STRUCTURE *board);
static int quiescence_search(int a, int b, BOARD_STRUCTURE *board, SEARCH_INFO_STRUCTURE *information);
void add_PV_move(const BOARD_STRUCTURE *board, const int move);
void emptyPvTable(PVTABLE_STRUCTURE *table);
void load_PV_table(PVTABLE_STRUCTURE *table);
void search_position(BOARD_STRUCTURE *board, SEARCH_INFO_STRUCTURE *infoStructure);

#endif //ZONA_CHESS_ENGINE_SEARCH_H