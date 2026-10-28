#!/bin/sh
for a in "$@"; do awk -v A="; ==== 0x$a" 'index($0, A) == 1 {p=1} p && /^$/ {exit} p' kit/code.txt | grep -v "  nop$" | cut -c1-120; done
