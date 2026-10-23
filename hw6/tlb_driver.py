import matplotlib.pyplot as plt
from pathlib import Path
from typing import List
import sys
import subprocess

def main():
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} <path_to_tlb_bin> <path_to_out_png>')
        return

    path_to_tlb: Path = Path(sys.argv[1]).resolve()
    path_to_png_out = Path(sys.argv[2])
    if not path_to_tlb.exists():
        print('Path to tlb binary incorrect')
        return
    if not path_to_png_out.parent.exists():
        print('Target path of graph png does not exist')
        return

    num_iters: int = 10000
    page_counts: List[int] = [2 ** p for p in range(15)] # 1 to 16384 pages
    costs: List[float] = []
    for pages in page_counts:
        cmd = [str(path_to_tlb), str(pages), str(num_iters)]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f'bin_tlb failed for {pages} pages: {result.stderr}')
            return
        for line in result.stdout.splitlines():
            if 'per page access' in line:
                costs.append(float(line.split()[-2]))
        print(f'Pages: {pages}, avg ns per access: {costs[-1]}')

    plt.plot(page_counts, costs, marker='o')

    plt.xscale('log', base=2)
    plt.xlabel('Number of pages accessed')
    plt.ylabel('Time per access (ns)')
    plt.title(f'TLB size measurement (num_iters={num_iters})')
    plt.grid(alpha=0.3)
    plt.savefig(path_to_png_out, dpi=150, bbox_inches='tight')

if __name__ == '__main__':
    main()
