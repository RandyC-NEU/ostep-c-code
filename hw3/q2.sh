#!/bin/bash

out=$(python3 ./lottery.py -l 10:1,10:100 -s 1 -c)
echo "seed 1 stats:"
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

# for each seed, count how many times job 0 runs before job 1 finishes
for seed in $(seq 1 1000); do
    python3 ./lottery.py -l 10:1,10:100 -s $seed -c \
        | awk '/JOB 1 DONE/ { print $NF, early; exit } /-> Run 0/ { early++ }'
done | awk '
    {
        n++; t1 += $1; e += $2
        if ($2 > 0) hit++
        if ($2 > max) max = $2
    }
    END {
        printf "1000 seeds stats:\n"
        printf "  seeds where job 0 ran before job 1 finished: %d (%.1f%%)\n", hit, 100 * hit / n
        printf "  avg job 0 runs before job 1 finished: %.3f\n", e / n
        printf "  max job 0 runs before job 1 finished: %d\n", max
        printf "  avg job 1 finish time: %.2f\n", t1 / n
    }'
