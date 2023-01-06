#!/usr/bin/env bash

set -x

OutputDir="/tmp"

rm -f $OutputDir/*.scad $OutputDir/*.stl

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
    --maze-complexity=8 > $OutputDir/version.scad
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
        > $OutputDir/$F.scad
    done
done

for w in 0 1 2 3;
do
    if [[ $w -eq 0 ]]; then
        PARTNAME=all
    else
        PARTNAME="$w-of-3"
    fi

    puzzlebox --parts=3 --core-height=30 --core-diameter=10 --outer-sides=5 --part=$w --inside \
    > $OutputDir/test-hard-part-$PARTNAME.scad

    puzzlebox --parts=3 --base-height=5 --core-height=20 --core-diameter=10 --outer-sides=5 --part=$w --flip --inside \
    > $OutputDir/test-flip-part-$PARTNAME.scad

    puzzlebox --parts=3 --inside --flip --core-solid --core-gap=10 --core-height=14 --core-diameter=24 --outer-sides=12 --part=$w --text-slow --text-depth=1 --text-end=' £ ' \
    > $OutputDir/five-pound-coins-part-$PARTNAME.scad

    puzzlebox --parts=3 --inside --flip --core-solid --core-gap=10 --core-height=28 --core-diameter=24 --outer-sides=12 --part=$w --text-slow --text-depth=1 --text-end=' £ ' \
    > $OutputDir/ten-pound-coins-part-$PARTNAME.scad

done

for w in 0 1 2;
do
    if [[ $w -eq 0 ]]; then
        PARTNAME=all
    else
        PARTNAME="$w-of-2"
    fi
    puzzlebox \
        --parts=2 \
        --core-height=85 \
        --core-gap=20 \
        --core-diameter=10 \
        --inside \
        --outer-sides=4 \
        --part=$w \
        --text-slow \
        --text-depth=1 \
        --text-end=' £ ' \
        --text-side="Good luck\\Prepare to\be amazed!" \
        --text-font="Mountains of Christmas" \
        --logo \
    > $OutputDir/lottery-ticket-hard-part-$PARTNAME.scad

    puzzlebox \
        --parts=2 \
        --core-height=85 \
        --core-gap=20 \
        --core-diameter=10 \
        --outer-sides=4 \
        --part=$w \
        --text-slow \
        --text-depth=1 \
        --text-end=' £ ' \
        --text-side="Good luck\\Prepare to\be amazed!" \
        --text-font="Mountains of Christmas" \
        --logo \
    > $OutputDir/lottery-ticket-easy-part-$PARTNAME.scad
done

# test maze complexity
for m in $(seq -10 10);
do
    puzzlebox \
    --base-height=5 \
    --parts=2 \
    --core-height=50 \
    --core-diameter=50 \
    --text-end=" $m " \
    --maze-complexity=$m \
    > $OutputDir/maze-complexity-$m.scad
done

for z in $(seq 3 8 );
do
    puzzlebox \
    --base-height=5 \
    --parts=2 \
    --core-height=20 \
    --core-diameter=40 \
    --outer-sides=6 \
    --maze-step=$z \
    > $OutputDir/maze-step-$z.scad
done

puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 > $OutputDir/all-test-six-outside.scad

puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 --logo --inside > $OutputDir/all-test-six-inside.scad

puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 --logo --flip > $OutputDir/all-test-six-outside-flip.scad

puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 --logo --flip --inside > $OutputDir/all-test-six-inside-flip.scad

puzzlebox \
    --parts=10 \
    --core-gap=10 \
    --core-diameter=20 \
    --core-height=50 \
    --logo \
    --text-slow \
    --text-depth=1 \
    --text-end='10\9\8\7\6\5\4\3\2\1' \
    > $OutputDir/all-ten.scad

# for scad in $(ls $OutputDir/*.scad);
# 	do
# 		echo $scad
# 		openscad -o $OutputDir/$(basename -s .scad $scad).png  $scad
# 		openscad -o $OutputDir/$(basename -s .scad $scad).stl  $scad
# 	done
# Usage: puzzlebox [OPTION...]
#   -m, --parts=N                         Total parts (default: 4)
#   -n, --part=N (0 for all)              Part to make
#   -i, --inside                          Maze on inside (hard)
#   -f, --flip                            Alternating inside/outside maze
#   -N, --nubs=N                          Nubs (default: 3)
#   -H, --helix=N (0 for non helical)     Helix (default: 3)
#   -b, --base-height=mm                  Base height (default: 10)
#   -c, --core-diameter=mm                Core diameter for content (default: 10)
#   -h, --core-height=mm                  Core height for content (default: 50)
#   -C, --core-gap=mm                     Core gap to allow content to be
#                                         removed (default: 0)
#   -q, --core-solid                      Core solid (content is in part 2)
#   -B, --base-thickness=mm               Base thickness (default: 1.6)
#   -G, --base-gap=mm                     Base gap (Z clearance) (default: 0.4)
#   -W, --base-wide                       Inside base full width
#   -w, --part-thickness=mm               Wall thickness (default: 1.2)
#   -t, --maze-thickness=mm               Maze thickness (default: 2)
#   -z, --maze-step=mm                    Maze spacing (default: 3)
#   -M, --maze-margin=mm                  Maze top margin (default: 1)
#   -X, --maze-complexity=-10 to 10       Maze complexity (default: 5)
#   -p, --park-thickness=mm               Thickness of park ridge to click
#                                         closed (default: 0.7)
#   -v, --park-vertical                   Park vertically
#   -g, --clearance=mm                    General X/Y clearance (default: 0.4)
#   -s, --outer-sides=N (0=round)         Number of outer sides (default: 7)
#   -r, --outer-round=mm                  Outer rounding on ends (default: 2)
#   -R, --grip-depth=mm                   Grip depth (default: 2)
#   -D, --text-depth=mm                   Text depth (default: 0.5)
#   -E, --text-end=X{\X...}               Text (initials) on end
#   -S, --text-side=Text{\Text...}        Text on sides
#   -F, --text-font=Font                  Text font (optional)
#   -e, --text-font-end=Font              Text font for end (optional)
#   -d, --text-slow                       Text has diagonal edges (very slow)
#   -T, --text-side-scale=N               Scale side text (i.e. if too long)
#   -O, --text-outset                     Text on sides is outset not embossed
#   -I, --text-inside=X{\X...}            Text (initials) inside end
#   -L, --logo-depth=mm                   Logo (and inside text) cut depth
#                                         (default: 0.6)
#   -V, --symmetric-cut                   Symmetric maze cut
#   -y, --nub-r-clearance=mm              Extra clearance on radius for nub
#                                         (default: 0.1)
#   -Z, --nub-z-clearance=mm              Extra clearance on height of nub
#                                         (default: 0.2)
#   -A, --logo                            Include A&A logo in last lid
#   -Q, --test                            Test pattern instead of maze
#       --mime                            MIME Header
#       --no-a                            No A
#       --web-form                        Web form
#
# Help options:
#   -?, --help                            Show this help message
#       --usage                           Display brief usage message
