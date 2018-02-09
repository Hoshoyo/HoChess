#pragma once
#include "chess.h"

struct Move_Evaluation {
	s8 src_rank, src_file;
	s8 dst_rank, dst_file;
	r32 evaluation;
};

Move_Evaluation maximize(Game_State* state, Board* board, r32 alpha, r32 beta, s32 depth_lvl);
Move_Evaluation minimize(Game_State* state, Board* board, r32 alpha, r32 beta, s32 depth_lvl);

r32 evaluate_position(Game_State* state, Board* board);

extern s32 node_count;