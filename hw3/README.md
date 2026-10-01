Bash scripts used to collect statistics for Chapter 9

"Lottery.py" was copied to this local directory to make running the experiments easier.

Run:
chmod +x ./q1.sh
chmod +x ./q2.sh
chmod +x ./q3.sh
chmod +x ./q5.sh

./q1.sh
./q2.sh
./q3.sh
./q5.sh

Note: Q5 produces a TSV compatible with gnuplot. To plot the data, use:
```
gnuplot -e "set terminal pngcairo; set output 'q5.png'; set logscale x; set xlabel 'job length'; set ylabel 'unfairness'; plot 'q5.dat' with linespoints; pause -1"
```

Required: python3/python3 alias for python
Optional: gnuplot (q5)
