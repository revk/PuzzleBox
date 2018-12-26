puzzlebox: puzzlebox.c
	-indent $<
	cc -O -o $@ $< -lpopt -lm -g -D_GNU_SOURCE
