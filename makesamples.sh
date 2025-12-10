#!/usr/bin/env bash

set -x

OutputDir="/home/puzzle/samples"

mkdir -vp $OutputDir

rm -f "$OutputDir/*.scad" "$OutputDir/*.stl" "$OutputDir/*.png"

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
        --text-end="$h\$w" \
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
    > "$OutputDir/test-hard-part-$PARTNAME.scad"

    puzzlebox --parts=3 --base-height=5 --core-height=20 --core-diameter=10 --outer-sides=5 --part=$w --flip --inside \
    > "$OutputDir/test-flip-part-$PARTNAME.scad"

    puzzlebox --parts=3 --inside --flip --core-solid --core-gap=10 --core-height=14 --core-diameter=24 --outer-sides=12 --part=$w --text-slow --text-depth=1 --text-end=' £ ' \
    > "$OutputDir/five-pound-coins-part-$PARTNAME.scad"

    puzzlebox --parts=3 --inside --flip --core-solid --core-gap=10 --core-height=28 --core-diameter=24 --outer-sides=12 --part=$w --text-slow --text-depth=1 --text-end=' £ ' \
    > "$OutputDir/ten-pound-coins-part-$PARTNAME.scad"

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
    > "$OutputDir/lottery-ticket-hard-part-$PARTNAME.scad"

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
    > "$OutputDir/lottery-ticket-easy-part-$PARTNAME.scad"
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
    --maze-complexity="$m" \
    > "$OutputDir/maze-complexity-$m.scad"
done

for z in $(seq 3 8 );
do
    puzzlebox \
    --base-height=5 \
    --parts=2 \
    --core-height=20 \
    --core-diameter=40 \
    --outer-sides=6 \
    --maze-step="$z" \
    > "$OutputDir/maze-step-$z.scad"
done

puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 > "$OutputDir/all-test-six-outside.scad"

puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 --logo --inside > "$OutputDir/all-test-six-inside.scad"

puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 --logo --flip > "$OutputDir/all-test-six-outside-flip.scad"

puzzlebox --parts=6 --core-gap=10 --core-diameter=12 --core-height=50 --logo --flip --inside > "$OutputDir/all-test-six-inside-flip.scad"

puzzlebox \
    --parts=10 \
    --core-gap=10 \
    --core-diameter=20 \
    --core-height=50 \
    --logo \
    --text-slow \
    --text-depth=1 \
    --text-end='10\9\8\7\6\5\4\3\2\1' \
    > "$OutputDir/all-ten.scad"

while IFS= read -r -d '' file
do
    ((count++))
    echo "Converting file number $count, $scad"
    openscad -o "$OutputDir/$(basename -s .scad $scad)".png "$scad"
    openscad -o "$OutputDir/$(basename -s .scad $scad)".stl "$scad"
done <   <(find $OutputDir -name '*.scad' -print0)
