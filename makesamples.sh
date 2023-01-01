#!/usr/bin/env bash

set -x

rm -f /tmp/*.scad /tmp/*.stl

F=version

puzzlebox \
	--parts=2 \
	--core-height=100 \
	--core-diameter=12 \
	--outer-sides=7 \
	--text-outset \
	--text-slow \
	--text-depth=0.8 \
	--text-end='AK' \
	--text-side="Test box http://www.me.uk/@TheRealRevK Build version date $(date --utc +%FT%TZ)\GitHub revk/PuzzleBox\Thingiverse 2410748" \
	--text-font=FiveByNineJTD \
	--text-font-end=FiveByNineJTD \
	--logo \
	--text-side-scale=0.85 \
	--maze-complexity=8 > /tmp/$F.scad
# Other boxes
for h in 0 1 2 3 ;
do
	for w in 0 1 2
		do
		F="test-helix-$h-part-$w-of-2"
		if [[ $w -eq 0 ]]; then
			F=test-helix-$h-all
		fi
		puzzlebox \
		--base-height=5 \
		--parts=2 \
		--core-height=15 \
		--core-diameter=10 \
		--part=$w \
		--text-end="$h\$h" \
		--helix=$h \
		> /tmp/$F.scad
	done
done

for w in 0 1 2 3;
do
	if [[ $w -eq 0 ]]; then
		PARTNAME=all
	else
		PARTNAME="$w-of-3"
	fi

	F=test-hard-part-$PARTNAME
	puzzlebox --parts=3 --core-height=30 --core-diameter=10 --outer-sides=5 --part=$w --inside > /tmp/$F.scad

	F=test-flip-part-$PARTNAME
	puzzlebox --parts=3 --base-height=5 --core-height=20 --core-diameter=10 --outer-sides=5 --part=$w --flip --inside > /tmp/$F.scad

	F=five-pound-coins-part-$PARTNAME
	puzzlebox --parts=3 --inside --flip --core-solid --core-gap=10 --core-height=14 --core-diameter=24 --outer-sides=12 --part=$w --text-slow --text-depth=1 --text-end=' £ ' > /tmp/$F.scad

	F=ten-pound-coins-part-$PARTNAME
	puzzlebox --parts=3 --inside --flip --core-solid --core-gap=10 --core-height=28 --core-diameter=24 --outer-sides=12 --part=$w --text-slow --text-depth=1 --text-end=' £ ' > /tmp/$F.scad

	if [[ $w -le 2 ]]; then
		F=lottery-ticket-hard-part-$PARTNAME
		puzzlebox --parts=2 --core-height=85 --core-gap=20 --core-diameter=10 --inside --outer-sides=4 --part=$w --text-slow --text-depth=1 --text-end=' £ ' --text-side="Good luck\\Prepare to\be amazed!" --text-font="Mountains of Christmas" --logo > /tmp/$F.scad

		F=lottery-ticket-easy-part-$PARTNAME
		puzzlebox --parts=2 --core-height=85 --core-gap=20 --core-diameter=10 --outer-sides=4 --part=$w --text-slow --text-depth=1 --text-end=' £ ' --text-side="Good luck\\Prepare to\be amazed!" --text-font="Mountains of Christmas" --logo $* > /tmp/$F.scad
	fi

done

# test maze complexity
for m in $(seq -10 10);
do
	F=maze-complexity-$m
	puzzlebox \
	--base-height=5 \
	--parts=2 \
	--core-height=50 \
	--core-diameter=50 \
	--text-end=" $m " \
	--maze-complexity=$m \
	> /tmp/$F.scad

done

F=all-test-six-outside
puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 > /tmp/$F.scad

F=all-test-six-inside
puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 --logo --inside > /tmp/$F.scad

F=all-test-six-outside-flip
puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 --logo --flip > /tmp/$F.scad

F=all-test-six-inside-flip
puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 --logo --flip --inside > /tmp/$F.scad

F=all-ten
puzzlebox --parts=10 --core-gap=10 --core-diameter=20 --core-height=50 --logo --text-slow --text-depth=1 --text-end='10\9\8\7\6\5\4\3\2\1' > /tmp/$F.scad

# for scad in $(ls /tmp/*.scad);
# 	do
# 		echo $scad
# 		openscad -o /tmp/$(basename -s .scad $scad).png  $scad
# 		openscad -o /tmp/$(basename -s .scad $scad).stl  $scad
# 	done