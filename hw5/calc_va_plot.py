import matplotlib.pyplot as plt
from pathlib import Path
from typing import List
import sys
import subprocess

def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <path_to_relocation_py> <path_to_out_png>")
        return

    path_to_relocation: Path = Path(sys.argv[1]).resolve()
    path_to_png_out = Path(sys.argv[2])
    if not path_to_relocation.exists():
        print("Path to relocation.py incorrect")
        return
    if not path_to_png_out.parent.exists():
        print("Target path of graph png does not exist")
    else:
        path_to_png_out.touch()

    one_k = 1024 # 1KB physical range
    n: int = 1000
    limits: List[int] = list(range(one_k+1))
    seeds = [1, 10, 100, 256, 512]
    histos = [[0] * (one_k+1)] * len(seeds)
    frac_valid = [[0] * (one_k+1)] * len(seeds)
    for h, s in enumerate(seeds):
        for lim in limits:
            args_str: str = '-s {seed} -n {n} -l {limit} -c'.format(seed=s, limit=lim, n=n)
            cmd = ['python3', str(path_to_relocation), *args_str.split(' ')]
            for line in subprocess.run(cmd, capture_output=True, text=True).stdout.splitlines():
                if '--> VALID' in line:
                    histos[h][lim] += 1
            frac_valid[h][lim] = histos[h][lim] / n
        print(f"Seed: {s}")
        print(frac_valid[h])
        print(histos[h])

    limits: List[int] = list(range(one_k+1))
    for seed, fracs in zip(seeds, frac_valid):
        plt.plot(limits, fracs, alpha=0.6, label=f'seed {seed}')
    plt.plot(limits, [l / one_k for l in limits], 'k--', label='Limit: limit / asize')

    plt.xlabel('Bounds (limit) register value (bytes)')
    plt.ylabel('Fraction of valid virtual addresses')
    plt.title(f'Valid VA fraction vs. bounds (asize=1024, n={n})')
    plt.xlim(0, one_k)
    plt.ylim(0, 1.02)
    plt.grid(alpha=0.3)
    plt.legend()
    plt.savefig(path_to_png_out, dpi=150, bbox_inches='tight')

if __name__ == '__main__':
    main()