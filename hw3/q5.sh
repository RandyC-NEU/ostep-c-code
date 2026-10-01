#!/bin/bash

# avg unfairness U vs job length, two jobs with 100 tickets each
# output: q5.dat with columns "length avg_U"

seeds=100

echo "# length avg_U" > q5.dat
for len in 1 2 5 10 20 50 100 200 500 1000; do
    for seed in $(seq 1 $seeds); do
        python3 ./lottery.py -l $len:100,$len:100 -s $seed -c \
            | awk '/JOB [0-9]+ DONE/ { t[n++] = $NF } END { print t[0] / t[1] }'
    done | awk -v len=$len '{ su += $1; n++ } END { printf "%d %.4f\n", len, su / n }' >> q5.dat
done

cat q5.dat
