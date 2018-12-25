puzzlebox: puzzlebox.c
	-indent $<
	cc -O -o $@ $< -lpopt
