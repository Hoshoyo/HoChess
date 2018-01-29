#include "common.h"
#include "chess.h"
#include "util.h"
#include "minmax.h"

extern s32 node_count;

void print_board(Game_State* state, bool flip = false);
void DEBUG_test_colors();
void select_valid_moves(Game_State* game_state, s32 rank, s32 file);
void clear_screen();

#define Continue clear_screen(); continue
#define ErrorContinue(X) clear_screen(); printf(X); continue
#define ErrorContinue2(X, Y) clear_screen(); printf(X, Y); continue

int main(int argc, char** argv)
{
	Game_State state;
	init_game_state(&state);
	board_clear(&state);
	board_init(&state);
	//board_put(&state, PIECE_BLACK_KING, 7, 4);
	//board_put(&state, PIECE_BLACK_QUEEN, 7, 3);
	//board_put(&state, PIECE_WHITE_KING, 0, 3);
	//board_put(&state, PIECE_WHITE_QUEEN, 1, 3);
	//board_put(&state, PIECE_BLACK_KNIGHT, 3, 3);

	Game_State previous_gs = state;
	while (true) {
		print_board(&state);
		printf("Position evaluation: %f\n", evaluate_position(&state));

		u32 length;
		s8* buffer = read_console(&length);

		// exit game
		if (string_equal("exit", buffer)) {
			break;
		}

		// go back one play
		if (string_equal("back", buffer)) {
			state = previous_gs;
			Continue;
		}

		// reset selection
		if (string_equal("reset", buffer)) {
			memset(state.board.selected, 0, sizeof(state.board.selected));
			Continue;
		}

		// make black to move
		if (string_equal("black", buffer)) {
			state.turn = PLAYER_BLACK;
			Continue;
		}

		// make white to move
		if (string_equal("white", buffer)) {
			state.turn = PLAYER_WHITE;
			Continue;
		}

		// setup the board with the starting position
		if (string_equal("startpos", buffer)) {
			init_game_state(&state);
			board_clear(&state);
			board_init(&state);
			Continue;
		}

		// clear the board
		if (string_equal("clear", buffer)) {
			board_clear(&state);
			Continue;
		}

		// select a particular square
		if (string_equal_until_length("select", buffer, sizeof("select") - 1)) {
			s8* square = buffer + sizeof("select") - 1;
			eat_spaces(&square);
			u32 len = strlen(square);
			if (len >= 2) {
				// select next
				s8 file = square[0] - 0x61;
				s8 rank = square[1] - 0x31;
				if (is_inside_board(rank, file)) {
					// reset previous selection
					memset(state.board.selected, 0, sizeof(state.board.selected));
					// select
					select_valid_moves(&state, rank, file);
				} else {
					ErrorContinue("Incorrect selection statement, must be inside the board.\n");
				}
			} else {
				ErrorContinue("Incorrect selection statement.\n");
			}
			Continue;
		}

		// put a piece on the board 
		// put BN c6 (Black Kight on c6)
		// put WK d8 (White King on d8)
		if (string_equal_until_length("put", buffer, sizeof("put") - 1)) {
			s8* piece = buffer + sizeof("put") - 1;
			eat_spaces(&piece);
			u32 len = strlen(piece);
			if (len > 2) {
				s8 color = piece[0];
				s8 p     = piece[1];
				Piece put_piece = PIECE_NONE;
				if (color == 'B' || color == 'W' || color == 'b' || color == 'w') {
					put_piece = get_piece_from_character(color, p);
					if (put_piece != PIECE_NONE) {
						s8* pos = piece + 2;
						eat_spaces(&pos);
						len = strlen(pos);
						if (len >= 2) {
							s8 file = pos[0] - 0x61;
							s8 rank = pos[1] - 0x31;
							if (is_inside_board(rank, file)) {
								board_put(&state, put_piece, rank, file);
							} else {
								ErrorContinue("Incorrect statement for putting piece, must be inside the board.\n");
							}
						} else {
							ErrorContinue("Incorrect statement for putting piece, must inform rank and file. Example: put BK c8\n");
						}
					} else {
						ErrorContinue2("Incorrect statement for putting piece, invalid piece '%c', must be (P, B, N, R, Q or K).\n", p);
					}
				} else {
					ErrorContinue("Incorrect statement for putting piece, must be B (Black) or W (White).\n");
				}
			} else {
				ErrorContinue("Incorrect statement for putting piece.\n");
			}
			Continue;
		}

		// remove piece from board
		if (string_equal_until_length("remove", buffer, sizeof("remove") - 1)) {
			s8* pos = buffer + sizeof("remove") - 1;
			eat_spaces(&pos);
			u32 len = strlen(pos);
			if (len >= 2) {
				s8 file = pos[0] - 0x61;
				s8 rank = pos[1] - 0x31;
				if (is_inside_board(rank, file)) {
					board_put(&state, PIECE_NONE, rank, file);
				} else {
					ErrorContinue("Incorrect statement for removing piece, must be inside board.\n");
				}
			} else {
				ErrorContinue("Incorrect statement for removing piece.\n");
			}
			Continue;
		}

		previous_gs = state;

		clear_screen();
		if (interpret_move(&state, buffer, length)) {
			printf("Last move: %s\n", buffer);
			switch_turn(&state);
			if (state.turn == PLAYER_BLACK) {
				Move_Evaluation value = minimize(state, -FLT_MAX, FLT_MAX, 0);
				printf("Node count: %d\n", node_count);
				node_count = 0;
				s8 engine_move[4] = {};
				fill_text_move(engine_move, value.src_rank, value.src_file, value.dst_rank, value.dst_file);
				interpret_move(&state, engine_move, 4);
				switch_turn(&state);
			}
		} else {
			printf("Invalid move\n");
		}
	}
	return 0;
}
