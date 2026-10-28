#!/bin/sh
# compact: drop machine word and nops
~/mmx-data/x6/re/x/weapons/scripts/fn.sh "$@" | sed -E 's/^([0-9a-f]{8}): [0-9a-f]{8}  /\1 /' | grep -v " nop$" | sed -E 's/^801//; s/  ; /\t; /'
