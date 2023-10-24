#!/bin/bash
set -eu

bam=$1
d=/share/home/liangwy/saturation
# ${d}/readmarker $bam | shuf | ${d}/saturation N | python ${d}/plot.py
#${d}/readmarker $bam && touch done.txt | shuf | (while [ ! -f done.txt ]; do sleep 0.5; done; ${d}/saturation N) | python ${d}/plot.py

# Clean up the sentinel file
#rm done.txt
#nohup time ./satu2 map/Aligned.sortedByCoord.out.bam 2> stats | python plot.py > time 2>> stats&
#time ${d}/satu2 $bam 2> stats | python ${d}/plot.py 2>> stats
time (
    ${d}/satu2 $bam 2> stats | python ${d}/plot.py 2>> stats
) 2>&1 
