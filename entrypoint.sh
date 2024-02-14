#!/usr/bin/env bash

# if no parameter return help
if [[ ${#@} -le 0 ]]; then
    echo "Call openSCAD with openscad <parameters>"
    echo "------------------"
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
    else
       puzzlebox --help
    fi 

else
    puzzlebox "${@}"

fi
