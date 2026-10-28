#!/bin/bash
code=$1; root=$2; cat=$3
cd ~
for f in $(ls ~/mmx-data/x6/dumps/*.state.gz | sort); do
  n=$(basename $f)
  PYTHONPATH=$code python3 -m mmxdemake.spriteexport --state $n --catalog $cat --root $root | tail -1
done
