#!/bin/sh
awk -v a="$1" 'BEGIN{a=tolower(a)} /^; ==== /{p=(index($3,a)==1)} p' ~/mmx-data/x6/re/x/kit/code.txt
