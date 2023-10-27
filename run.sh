#!/bin/bash
set -eu

bam=$1
d=$(dirname "$0")
time (
    ${d}/satu2 $bam 2> stats | python ${d}/plot.py 2>> stats
) 2>&1 
