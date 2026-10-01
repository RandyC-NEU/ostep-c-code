#!/bin/bash

for seed in 1 2 3; do
    out=$(python3 ./lottery.py -j 3 -s $seed -c)
    echo "seed $seed stats:"
    echo "$out" | awk '
        /Job [0-9]+ \( length/ {
            gsub(/,/, ""); len[$2] = $6; tix[$2] = $9; total += $9; n++
        }
        /JOB [0-9]+ DONE/ { done[$3] = $NF }
        END {
            for (j = 0; j < n; j++)
                printf "  job %d: length %s, tickets %s (%.1f%%), done at %s\n",
                    j, len[j], tix[j], 100 * tix[j] / total, done[j]
        }'
    echo
done
