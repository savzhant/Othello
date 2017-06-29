#pragma once
#include "Board.h"

struct Heuristic {
	static const int weight[8][8];
	static int eval(const Board& oth);
	static int staticEval(const Board& oth);
};