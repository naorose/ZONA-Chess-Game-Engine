
#ifndef ZONA_CHESS_ENGINE_PERFT_H
#define ZONA_CHESS_ENGINE_PERFT_H

#include <stdio.h>
#include "structures.h"
#include "board.h"
#include "moveGenerator.h"
#include "moveMaker.h"
#include "inputOutput.h"
#include "miscellaneous.h"
void perft(int depth, BOARD_STRUCTURE *board);
void perft_test(int depth, BOARD_STRUCTURE *board);
#endif //ZONA_CHESS_ENGINE_PERFT_H
