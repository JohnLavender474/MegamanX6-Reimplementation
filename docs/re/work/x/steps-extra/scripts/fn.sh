#!/bin/sh
for a in "$@"; do
awk -v a="; ==== 0x$a" 'index($0, a)==1{p=1} p&&/^$/{exit} p' "${KIT:-$HOME/mmx-data/x6/re/x/kit/code.txt}" | grep -v -E "^\S+: 00000000  nop$"
echo
done
