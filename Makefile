puzzlebox: puzzlebox.c
	cc -O -o $@ $< -lpopt -lm -g -D_GNU_SOURCE
