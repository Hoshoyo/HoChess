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
#include <unistd.h>
s8* read_console(u32* out_length) {
	int f = dup(STDIN_FILENO);
	size_t size = read(f, console_buffer, 2048);
	*out_length = size;
	while(size == 2048){
		s8 dump_buffer[2048];
		size = read(f, dump_buffer, sizeof(dump_buffer));
	}
	close(f);
	return console_buffer;
}
#endif
