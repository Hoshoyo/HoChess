#include "minmax.h"
#include <assert.h>

r32 piece_value[NUM_PIECES] = {
	0,  1,  200,  9,  5,  3,  3, 
	   -1,  -200, -9, -5, -3, -3
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

r32 evaluate_position(Game_State* state) {
	r32 value = 0;
	for (s32 rank = 0; rank < 8; ++rank) {
		for (s32 file = 0; file < 8; ++file) 
		{
			Piece p = state->board.piece[rank][file];
			r32 static_bonus = 0.0f;
			switch (p) {
				case PIECE_WHITE_PAWN: {
					static_bonus = 0.1 * white_pawn_static_weight[rank][file];
					break;
				}
				case PIECE_BLACK_PAWN: {
					static_bonus = -0.1 * black_pawn_static_weight[rank][file]; 
					break;
				}
			}

			value += piece_value[p] + static_bonus;
		}
	}
	return value;
}

const s32 depth_level = 8;

s32 node_count = 0;

Move_Evaluation minimize(Game_State state, r32 alpha, r32 beta, s32 depth_lvl) {
	if (depth_lvl == depth_level) {
		Move_Evaluation move = {};
		move.evaluation = evaluate_position(&state);
		++node_count;
		return move;;
	}
	//r32 v = FLT_MAX;
	Move_Evaluation v = {};
	v.evaluation = FLT_MAX;

	for (s32 rank = 0; rank < 8; ++rank)
	{
		for (s32 file = 0; file < 8; ++file)
		{
			if (state.turn == PLAYER_BLACK) {
				switch (state.board.piece[rank][file]) {
					
					case PIECE_BLACK_BISHOP: {
						for(s32 i = rank + 1; i < 8; ++i){
							for(s32 j = file + 1; j < 8; ++j){
								Piece p = state.board.piece[i][j];
								if(p == PIECE_NONE){
									// move to empty square
								} else if (is_white_piece(p)) {
									goto end_bishop_top_right;
								} else if(is_black_piece(p)) {
									// capture
								}
							}
						}
						
						end_bishop_top_right:
						
						for(s32 i = rank + 1; i < 8; ++i){
							for(s32 j = file - 1; j >= 0; --j){
								Piece p = state.board.piece[i][j];
								if(p == PIECE_NONE){
									// move to empty square
								} else if (is_white_piece(p)) {
									goto end_bishop_top_left;
								} else if(is_black_piece(p)) {
									// capture
								}
							}
						}

						end_bishop_top_left:

						for(s32 i = rank - 1; i >= 0; --i){
							for(s32 j = file + 1; j < 8; ++j){
								Piece p = state.board.piece[i][j];
								if(p == PIECE_NONE){
									// move to empty square
								} else if (is_white_piece(p)) {
									goto end_bishop_bot_right;
								} else if(is_black_piece(p)) {
									// capture
								}
							}
						}

						end_bishop_bot_right:

						for(s32 i = rank - 1; i >= 0; --i){
							for(s32 j = file - 1; j >= 0; --j){
								Piece p = state.board.piece[i][j];
								if(p == PIECE_NONE){
									// move to empty square
								} else if (is_white_piece(p)) {
									goto end_bishop_bot_left;
								} else if(is_black_piece(p)) {
									// capture
								}
							}
						}

						end_bishop_bot_left:
						int x = 0;

					} break;

					case PIECE_BLACK_PAWN: {
						// captures
						s32 move = is_valid(&state, rank, file, rank - 1, file + 1);
						if (move == MOVE_VALID) {
							Game_State s = state;
							pawn_capture_unchecked(&s, rank, file, rank - 1, file + 1);
							switch_turn(&s);
							r32 v_prime = maximize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime < v.evaluation) {
								v.src_file = file;
								v.dst_file = file + 1;
								v.src_rank = rank;
								v.dst_rank = rank - 1;
								v.evaluation = v_prime;
							}
							if (v_prime <= alpha) return v;
							if (v_prime < beta) beta = v_prime;
						} else if (move == MOVE_VALID_EN_PASSANT) {
							Game_State s = state;
							pawn_capture_en_passant_unchecked(&s, rank, file, rank - 1, file + 1);
							switch_turn(&s);
							r32 v_prime = maximize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime < v.evaluation) {
								v.src_file = file;
								v.dst_file = file + 1;
								v.src_rank = rank;
								v.dst_rank = rank - 1;
								v.evaluation = v_prime;
							}
							if (v_prime <= alpha) return v;
							if (v_prime < beta) beta = v_prime;
						}

						move = is_valid(&state, rank, file, rank - 1, file - 1);
						if (move == MOVE_VALID) {
							Game_State s = state;
							pawn_capture_unchecked(&s, rank, file, rank - 1, file - 1);
							switch_turn(&s);
							r32 v_prime = maximize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime < v.evaluation) {
								v.src_file = file;
								v.dst_file = file - 1;
								v.src_rank = rank;
								v.dst_rank = rank - 1;
								v.evaluation = v_prime;
							}
							if (v_prime <= alpha) return v;
							if (v_prime < beta) beta = v_prime;
						} else if (move == MOVE_VALID_EN_PASSANT) {
							Game_State s = state;
							pawn_capture_en_passant_unchecked(&s, rank, file, rank - 1, file - 1);
							switch_turn(&s);
							r32 v_prime = maximize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime < v.evaluation) {
								v.src_file = file;
								v.dst_file = file - 1;
								v.src_rank = rank;
								v.dst_rank = rank - 1;
								v.evaluation = v_prime;
							}
							if (v_prime <= alpha) return v;
							if (v_prime < beta) beta = v_prime;
						}

						// advance twice
						move = is_valid(&state, rank, file, rank - 2, file);
						if (move == MOVE_VALID) {
							Game_State s = state;
							pawn_push_unchecked(&s, rank, file, rank - 2, file);
							switch_turn(&s);
							r32 v_prime = maximize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime < v.evaluation) {
								v.src_file = file;
								v.dst_file = file;
								v.src_rank = rank;
								v.dst_rank = rank - 2;
								v.evaluation = v_prime;
							}
							if (v_prime <= alpha) return v;
							if (v_prime < beta) beta = v_prime;
						}
						// advance once
						if (is_valid(&state, rank, file, rank - 1, file)) {
							Game_State s = state;
							pawn_push_unchecked(&s, rank, file, rank - 1, file);
							switch_turn(&s);
							r32 v_prime = maximize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime < v.evaluation) {
								v.src_file = file;
								v.dst_file = file;
								v.src_rank = rank;
								v.dst_rank = rank - 1;
								v.evaluation = v_prime;
							}
							if (v_prime <= alpha) return v;
							if (v_prime < beta) beta = v_prime;
						}
					}break;
				}
			} else {
				assert(0);
			}
		}
	}
	return v;
}

Move_Evaluation maximize(Game_State state, r32 alpha, r32 beta, s32 depth_lvl) {
	if(depth_lvl == depth_level){
		Move_Evaluation move = {};
		move.evaluation = evaluate_position(&state);
		++node_count;
		return move;;
	}
	//r32 v = -FLT_MAX;
	Move_Evaluation v = {};
	v.evaluation = -FLT_MAX;

	for (s32 rank = 0; rank < 8; ++rank) 
	{
		for (s32 file = 0; file < 8; ++file) 
		{
			if (state.turn == PLAYER_WHITE) {
				switch (state.board.piece[rank][file]) {

					case PIECE_WHITE_PAWN: {
						// captures
						s32 move = is_valid(&state, rank, file, rank + 1, file + 1);
						if (move == MOVE_VALID) {
							Game_State s = state;
							pawn_capture_unchecked(&s, rank, file, rank + 1, file + 1);
							switch_turn(&s);
							r32 v_prime = minimize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime > v.evaluation) {
								v.src_file = file;
								v.dst_file = file + 1;
								v.src_rank = rank;
								v.dst_rank = rank + 1;
								v.evaluation = v_prime;
							}
							if (v_prime > alpha) alpha = v_prime;
						} else if (move == MOVE_VALID_EN_PASSANT) {
							Game_State s = state;
							pawn_capture_en_passant_unchecked(&s, rank, file, rank + 1, file + 1);
							switch_turn(&s);
							r32 v_prime = minimize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime > v.evaluation) {
								v.src_file = file;
								v.dst_file = file + 1;
								v.src_rank = rank;
								v.dst_rank = rank + 1;
								v.evaluation = v_prime;
							}
							if (v_prime >= beta) return v;
							if (v_prime > alpha) alpha = v_prime;
						}

						move = is_valid(&state, rank, file, rank + 1, file - 1);
						if (move == MOVE_VALID) {
							Game_State s = state;
							pawn_capture_unchecked(&s, rank, file, rank + 1, file - 1);
							switch_turn(&s);
							r32 v_prime = minimize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime > v.evaluation) {
								v.src_file = file;
								v.dst_file = file - 1;
								v.src_rank = rank;
								v.dst_rank = rank + 1;
								v.evaluation = v_prime;
							}
							if (v_prime >= beta) return v;
							if (v_prime > alpha) alpha = v_prime;
						} else if (move == MOVE_VALID_EN_PASSANT) {
							Game_State s = state;
							pawn_capture_en_passant_unchecked(&s, rank, file, rank + 1, file - 1);
							switch_turn(&s);
							r32 v_prime = minimize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime > v.evaluation) {
								v.src_file = file;
								v.dst_file = file - 1;
								v.src_rank = rank;
								v.dst_rank = rank + 1;
								v.evaluation = v_prime;
							}
							if (v_prime >= beta) return v;
							if (v_prime > alpha) alpha = v_prime;
						}

						// advance twice
						move = is_valid(&state, rank, file, rank + 2, file);
						if (move == MOVE_VALID) {
							Game_State s = state;
							pawn_push_unchecked(&s, rank, file, rank + 2, file);
							switch_turn(&s);
							r32 v_prime = minimize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime > v.evaluation) {
								v.src_file = file;
								v.dst_file = file;
								v.src_rank = rank;
								v.dst_rank = rank + 2;
								v.evaluation = v_prime;
							}
							if (v_prime >= beta) return v;
							if (v_prime > alpha) alpha = v_prime;
						}
						// advance once
						if (is_valid(&state, rank, file, rank + 1, file)) {
							Game_State s = state;
							pawn_push_unchecked(&s, rank, file, rank + 1, file);
							switch_turn(&s);
							r32 v_prime = minimize(s, alpha, beta, depth_lvl + 1).evaluation;
							if (v_prime > v.evaluation) {
								v.src_file = file;
								v.dst_file = file;
								v.src_rank = rank;
								v.dst_rank = rank + 1;
								v.evaluation = v_prime;
							}
							if (v_prime >= beta) return v;
							if (v_prime > alpha) alpha = v_prime;
						}
					}break;

				}
			} else {
				assert(0);
			}
		}
	}
	return v;
}
