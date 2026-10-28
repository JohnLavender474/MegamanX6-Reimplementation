#!/bin/sh
for a in "$@"; do awk -v a="$a" 'BEGIN{a=tolower(a)} /^; ==== /{p=(index($3,a)==1)} p' ~/mmx-data/x6/re/x/weapons/kit/code.txt; done
