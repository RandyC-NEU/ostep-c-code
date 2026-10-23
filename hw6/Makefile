CC = gcc
CFLAGS = -Wall -Wpedantic -O0

.PHONY: all clean

all: build_tlb

build_tlb: tlb.c
	$(CC) $(CFLAGS) -o bin_tlb tlb.c

clean:
	rm bin*
