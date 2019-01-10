# Puzzle Box
Makes OpenSCAD file for a puzzle box

Has been tested on linux, you probably need libpopt-dev

As you will see from the Makefile, you will need popt development library.
You also need gcc supporting _GNU_SOURCE for strdupa() and asprintf()

And, for the avoidance of doubt, obviously, this is normally built using "make"

When built, use --help for options, or see https://www.me.uk/puzzlebox

Does not build on Mac - but if anyone knows how to get gcc extensions working on a Mac, that would be a great update which I'd be happy to apply.
