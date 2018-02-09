#include "minimax.h"
#include <assert.h>

r32 piece_value[16] = {
	0,  
	1,  3, 3,  5,  9,  200,		// white
	0, 0,
	-1, -3, -3, -5, -9, -200	// black
};

r32 black_pawn_static_weight[8][8] = {
	{ 20,20,20,20,20,20,20,20 },
	{ 8,8,8,8,8,8,8,8 },
	{ 5,5,5,5,5,5,5,5 },
	{ 4,4,4,4,4,4,4,4 },
	{ 1,1,3,4,4,3,1,1 },
	{ 1,1,2,2,2,0,0,1 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
};

r32 white_pawn_static_weight[8][8] = {
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 1,1,2,2,2,0,0,1 },
	{ 1,1,3,4,4,3,1,1 },
	{ 4,4,4,4,4,4,4,4 },
	{ 5,5,5,5,5,5,5,5 },
	{ 8,8,8,8,8,8,8,8 },
	{ 20,20,20,20,20,20,20,20 },
};

r32 queen_mobility_weight[8][8] = {
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 1,1,2,2,2,0,0,1 },
	{ 1,1,3,4,4,3,1,1 },
	{ 4,4,4,4,4,4,4,4 },
	{ 5,5,5,5,5,5,5,5 },
	{ 8,8,8,8,8,8,8,8 },
	{ 0,0,0,0,0,0,0,0 },
};

r32 evaluate_position(Game_State* state, Board* board) {
	r32 value = 0;
	for (s32 rank = 0; rank < 8; ++rank) {
		for (s32 file = 0; file < 8; ++file)
		{
			Piece p = board->square[rank][file];
			r32 static_bonus = 0.0f;
			switch (p) {
				case PIECE_WHITE_PAWN: {
					static_bonus = 0.1 * white_pawn_static_weight[rank][file];
				} break;
				case PIECE_BLACK_PAWN: {
					static_bonus = -0.1 * black_pawn_static_weight[rank][file];
				} break;
			}

			value += piece_value[p] + static_bonus;
		}
	}
	return value;
}

u32 depth_level = 7;
s32 node_count  = 0;
#include <float.h>

Move_Evaluation maximize(Game_State* state, Board* board, r32 alpha, r32 beta, s32 depth_lvl);

bool move_equal(Move_Evaluation m) {
	Move m2 = {};
	m2.src_rank = 0;
	m2.src_file = 2;
	m2.dst_rank = 5;
	m2.dst_file = 7;

	return (m2.src_rank == m.src_rank &&
		m2.dst_rank == m.dst_rank &&
		m2.src_file == m.src_file &&
		m2.dst_file == m.dst_file);
}
#include "ui.h"

Move_Evaluation minimize(Game_State* state, Board* board, r32 alpha, r32 beta, s32 depth_lvl) {
	if (depth_lvl >= depth_level) {
		Move_Evaluation move = {};
		move.evaluation = evaluate_position(state, board);
		++node_count;
		return move;
	}
	Move_Evaluation v = {};
	v.evaluation = FLT_MAX;

	Move moves[1024];
	u32 moves_count = 0;
	generate_possible_moves(state, board, moves, &moves_count);

	for (u32 i = 0; i < moves_count; ++i) {
		// do move
		Game_State last_state = *state;
		do_move(state, board, moves[i]);

		r32 v_prime = maximize(state, board, alpha, beta, depth_lvl + 1).evaluation;

		// undo move
		undo_move(last_state, state, board, moves[i]);

		if (v_prime < v.evaluation) {
			v.src_file = moves[i].src_file;
			v.dst_file = moves[i].dst_file;
			v.src_rank = moves[i].src_rank;
			v.dst_rank = moves[i].dst_rank;
			v.evaluation = v_prime;
		}
		if (v_prime <= alpha) return v;
		if (v_prime < beta) beta = v_prime;
	}

	return v;
}

Move_Evaluation maximize(Game_State* state, Board* board, r32 alpha, r32 beta, s32 depth_lvl) {
	if (depth_lvl >= depth_level) {
		Move_Evaluation move = {};
		move.evaluation = evaluate_position(state, board);
		++node_count;
		return move;
	}
	Move_Evaluation v = {};
	v.evaluation = -FLT_MAX;

	Move moves[1024];
	u32 moves_count = 0;
	generate_possible_moves(state, board, moves, &moves_count);

	for (u32 i = 0; i < moves_count; ++i) {
		// do move
		Game_State last_state = *state;
		do_move(state, board, moves[i]);

		r32 v_prime = minimize(state, board, alpha, beta, depth_lvl + 1).evaluation;

		// undo move
		undo_move(last_state, state, board, moves[i]);

		if (v_prime > v.evaluation) {
			v.src_file = moves[i].src_file;
			v.dst_file = moves[i].dst_file;
			v.src_rank = moves[i].src_rank;
			v.dst_rank = moves[i].dst_rank;
			v.evaluation = v_prime;
		}
		if (v_prime >= beta) return v;
		if (v_prime > alpha) alpha = v_prime;
	}

	return v;
}