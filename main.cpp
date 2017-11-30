#include "common.h"
#include "chess.h"
#include "util.h"

void print_board(Game_State* state, bool flip = false);
void print_board_marks(Game_State* state, bool flip = false);
void DEBUG_test_colors();

int main(int argc, char** argv)
{
	Piece P = PIECE_BLACK_KNIGHT_0;
	s32 row = 4;
	s32 col = 4;

	Game_State state;
	board_clear(&state);
	board_put(&state, P, row, col);

	board_put(&state, PIECE_WHITE_PAWN_0, 4, 0);


	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (is_valid(&state, P, i, j)) {
				board_mark(&state, i, j);
			}
		}
	}
	while (true) {
		print_board(&state);
		print_board_marks(&state);
		u32 length;
		s8* buffer = read_console(&length);
		board_mark_clear(&state);
		
		s32 i = state.piece_state[P].line;
		s32 j = state.piece_state[P].column;
		bool valid = is_valid(&state, P, i - 1, j + 1);
		if (valid) {
			board_put(&state, PIECE_NONE, i, j);
			board_put(&state, P, i - 1, j + 1);
		}
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (is_valid(&state, P, i, j)) {
					board_mark(&state, i, j);
				}
			}
		}
	}
	return 0;
}