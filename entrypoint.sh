#!/usr/bin/env bash

# if no parameter return help
if [[ ${#@} -le 0 ]]; then
    echo "Call puzzlebox with puzzlebox <parameters>"
    echo "e.g., puzzlebox --stl --core-diameter=40 --core-height=15 --text-end=\"Surprise\""
    echo " or just the parameters, e.g., --stl --core-diameter=40 --core-height=15 --text-end=\"Surprise\""
    echo "------------------"
    echo "Call openSCAD with openscad <parameters>"
    echo "e.g., openscad -o output.stl input.scad"
    echo "------------------"
    echo 
    puzzlebox --help

elif [[ "${*}" == "bash" ]]; then
    bash

elif [[ "$1" == "openscad" ]]; then
    shift
    if [[ ${#@} -gt 0 ]]; then
       openscad "${@}"
    else
       openscad --help
    fi 

elif [[ "$1" == "puzzlebox" ]]; then
    shift
    if [[ ${#@} -gt 0 ]]; then
       puzzlebox "${@}"
    fi 

else
    puzzlebox "${@}"
    exit
fi
