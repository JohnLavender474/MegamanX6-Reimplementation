#!/bin/bash
A=$1
rm -rf $A; mkdir -p $A
cd ~/IdeaProjects/mmx-demake-tools
for n in commander-yammark rocket-platform shield-sheldon meta-wheel-f boss-gate; do python3 -m mmxdemake.rekit --name $n --out $A/$n 2>&1 | tail -1; done
python3 -m mmxdemake.rekit --name nightmare-shadow --out $A/shadow | tail -1
python3 -m mmxdemake.rekit --state shark-7_boss.state.gz --type 56 --out $A/shark 2>&1|tail -1
python3 -m mmxdemake.rekit --state shark-4_mini-boss.state.gz --type 16 --out $A/pressure 2>&1|tail -1
python3 -m mmxdemake.rekit --player --state intro-1_start.state.gz --out $A/player-x 2>&1 | tail -1
cd ~/mmx-data/x6/re/kit-fixes && for d in before/*/; do n=$(basename $d); [ -d $A/$n ] || continue; echo "== $n $(diff -r -I '^- State:' before/$n $A/$n | grep -c '^[<>]')"; done
