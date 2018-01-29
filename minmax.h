#pragma once
#include "common.h"
#include "chess.h"
#include <float.h>

struct Move_Evaluation {
	s32 src_rank, src_file;
	s32 dst_rank, dst_file;
	r32 evaluation;
};

Move_Evaluation maximize(Game_State state, r32 alpha, r32 beta, s32 depth_lvl);
Move_Evaluation minimize(Game_State state, r32 alpha, r32 beta, s32 depth_lvl);
r32 evaluate_position(Game_State* state);