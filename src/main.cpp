#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "common.h"
#include "ui.h"
#include "chess.h"
#include "minimax.h"
#include "util.h"
#include <float.h>

#define Continue clear_screen(); continue
#define ErrorContinue(X) clear_screen(); printf(X); continue
#define ErrorContinue2(X, Y) clear_screen(); printf(X, Y); continue

int main(int argc, char** argv) 
{
	Board board = {};
	Game_State state = {};
	Board_Select b_select = 0;

	game_state_init(&state);
	board_startpos(&board);
	
	//parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &board, &state);
	//parse_fen("r1bqk2r/p2nppbp/2pp1np1/1p6/3PP3/2N1BP2/PPPQN1PP/R3KB1R w KQkq 4 5", &board, &state);
	//parse_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq -", &board, &state);

	Game_State previous_gs = state;
	Board previous_board = board;
	while (true) 
	{
		print_board(&board, b_select);
		printf("Position evaluation: %f\n", evaluate_position(&state, &board));

		u32 length;
		s8* buffer = read_console(&length);

		// exit game
		if (string_equal("exit", buffer)) {
			break;
		}

		// go back one play
		if (string_equal("back", buffer)) {
			state = previous_gs;
			board = previous_board;
			Continue;
		}

		// reset selection
		if (string_equal("reset", buffer)) {
			b_select = 0;
			Continue;
		}

		// make black to move
		if (string_equal("black", buffer)) {
			state.flags |= GAME_STATE_BLACK_TO_MOVE;
			state.flags &= ~GAME_STATE_WHITE_TO_MOVE;
			Continue;
		}

		// make white to move
		if (string_equal("white", buffer)) {
			state.flags |= GAME_STATE_WHITE_TO_MOVE;
			state.flags &= ~GAME_STATE_BLACK_TO_MOVE;
			Continue;
		}

		// setup the board with the starting position
		if (string_equal("startpos", buffer)) {
			game_state_init(&state);
			board_startpos(&board);
			Continue;
		}

		// clear the board
		if (string_equal("clear", buffer)) {
			board_clear(&board);
			game_state_init(&state);
			Continue;
		}

		if (string_equal_until_length("fen", buffer, sizeof("fen") - 1)) {
			s8* fen = buffer + sizeof("fen") - 1;
			eat_spaces(&fen);
			u32 len = strlen(fen);
			parse_fen(fen, &board, &state);
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
					b_select = 0;
					// select
					piece_select(&board, &state, &b_select, rank, file, 0);
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
				s8 p = piece[1];
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
								board_put(&board, rank, file, put_piece);
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
					board_put(&board, rank, file, PIECE_NONE);
				} else {
					ErrorContinue("Incorrect statement for removing piece, must be inside board.\n");
				}
			} else {
				ErrorContinue("Incorrect statement for removing piece.\n");
			}
			Continue;
		}


		previous_gs = state;
		previous_board = board;

		s8 engine_move[8] = {};
		if (interpret_move(&state, &board, buffer, length)) {
			printf("Last move: %s\n", buffer);
			if (is_black_turn(&state)) {
				Move_Evaluation value = minimize(&state, &board, -FLT_MAX, FLT_MAX, 0);
				fill_text_move(engine_move, value.src_rank, value.src_file, value.dst_rank, value.dst_file);
				interpret_move(&state, &board, engine_move, strlen(engine_move));
			}
		} else {
			printf("Invalid move\n");
		}

		clear_screen();
		printf("Node count: %d\n", node_count);
		printf("Engine move: %s\n", engine_move);
		node_count = 0;
	}

	return 0;
}
