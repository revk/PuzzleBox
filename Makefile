puzzlebox: puzzlebox.c
	ifeq ($(shell uname),Darwin)
	/usr/local/opt/gcc/bin/gcc-8 -L/usr/local/lib -I/usr/local/include -O -o $@ $< -lpopt -lm -g -D_GNU_SOURCE
	else
	cc -O -o $@ $< -lpopt -lm -g -D_GNU_SOURCE
	endif
