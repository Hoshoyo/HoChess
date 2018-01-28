#include "minmax.h"
#include <assert.h>

s32 piece_value[NUM_PIECES] = {
	0,  1,  INT_MAX,  9,  5,  3,  3, 
	   -1, -INT_MAX, -9, -5, -3, -3
};

s32 evaluate_position(Game_State* state) {
	s32 value = 0;
	for (s32 rank = 0; rank < 8; ++rank) {
		for (s32 file = 0; file < 8; ++file) {
			value += piece_value[state->board.piece[rank][file]];
		}
	}
	return value;
}

s32 maximize(Game_State state, s32 alpha, s32 beta, s32 depth_lvl) {
	if(depth_lvl == 3){
		return evaluate_position(&state);
	}
	for (s32 rank = 0; rank < 8; ++rank) {
		for (s32 file = 0; file < 8; ++file) {
			if (state.turn == PLAYER_BLACK) {
				switch (state.board.piece[rank][file]) {
				case PIECE_BLACK_PAWN: {

				}break;
				}
			} else {
				assert(0);
			}
		}
	}
}