#include "util.h"

s8 console_buffer[2048] = { };

#if defined(_WIN32) || defined(_WIN64)
s8* read_console(u32* out_length) {
	DWORD read;
	ReadConsole(
		GetStdHandle(STD_INPUT_HANDLE),
		console_buffer,
		sizeof(console_buffer),
		&read, 0
	);
	*out_length = read;
	while (read == 2048) {
		s8 dump_buffer[2048];
		ReadConsole(
			GetStdHandle(STD_INPUT_HANDLE),
			dump_buffer,
			sizeof(dump_buffer),
			&read, 0
		);
	}
	return console_buffer;
}

#elif defined(__linux__)

#endif