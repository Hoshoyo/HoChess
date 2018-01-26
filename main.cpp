#include "common.h"
#include "chess.h"
#include "util.h"

void print_board(Game_State* state, bool flip = false);
void DEBUG_test_colors();

int main(int argc, char** argv)
{
	Game_State state;
	init_game_state(&state);
	board_clear(&state);
	//board_init(&state);
	board_put(&state, PIECE_BLACK_KING, 7, 4);
	board_put(&state, PIECE_BLACK_QUEEN, 7, 3);
	board_put(&state, PIECE_WHITE_KING, 0, 3);
	board_put(&state, PIECE_WHITE_QUEEN, 1, 3);

	while (true) {
		print_board(&state);
		u32 length;
		s8* buffer = read_console(&length);

		if (interpret_move(&state, buffer, length)) {
			//switch_turn(&state);
			printf("Valid move\n");
		} else {
			printf("Invalid move\n");
		}
	}
	return 0;
}
