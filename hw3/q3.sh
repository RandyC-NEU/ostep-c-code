#!/bin/bash

# unfairness U = first finish time / second finish time
for seed in $(seq 1 1000); do
    python3 ./lottery.py -l 100:100,100:100 -s $seed -c \
        | awk -v s=$seed '/JOB [0-9]+ DONE/ { t[n++] = $NF } END { print s, t[0], t[1] }'
done | awk '
    {
        u = $2 / $3; gap = $3 - $2
        if (NR <= 10)
            printf "seed %d: first done at %d, second at %d, gap %d, U %.3f\n", $1, $2, $3, gap, u
        n++; su += u; sg += gap
        if (n == 1 || u < umin) umin = u
        if (u > umax) umax = u
        if (gap > gmax) gmax = gap
    }
    END {
        printf "\n1000 seeds stats:\n"
        printf "  avg U: %.3f\n", su / n
        printf "  min U: %.3f\n", umin
        printf "  max U: %.3f\n", umax
        printf "  avg gap: %.2f\n", sg / n
        printf "  max gap: %d\n", gmax
    }'
