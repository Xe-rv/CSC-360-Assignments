#! /usr/bin/bash
for i in $(seq 1 10);
do
    ran=$((1 + $RANDOM % 100))
    echo
    echo sem:
    ./kosmos-sem ran $ran
    echo
    echo mcv:
    ./kosmos-mcv ran $ran
done