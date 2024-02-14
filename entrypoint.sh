#!/usr/bin/env bash

# if no parameter return help
if [[ ${#@} -le 0 ]]; then
    puzzlebox --help

elif [[ "${*}" == "bash" ]]; then
    bash

elif [[ "$1" == "openscad" ]]; then
    shift
    openscad "${@}"

else
    puzzlebox "${@}"

fi
