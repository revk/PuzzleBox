# Puzzle Box
Makes OpenSCAD file for a puzzle box

Has been tested on linux, you probably need libpopt-dev

As you will see from the Makefile, you will need popt development library.
You also need gcc supporting _GNU_SOURCE for strdupa() and asprintf()

And, for the avoidance of doubt, obviously, this is normally built using "make"

When built, use --help for options, or see https://www.me.uk/puzzlebox

On macOS, this requires gcc, so "brew install gcc" first and change the makefile to use another compiler, like /usr/local/opt/gcc/bin/gcc-8
