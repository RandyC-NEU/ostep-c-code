CC = gcc
CFLAGS = -Wall -Wpedantic -O0

.PHONY: all clean

all: build_5 build_6

build_5: ch5.c
	$(CC) $(CFLAGS) -o bin5 ch5.c

build_6: ch6.c
	$(CC) $(CFLAGS) -o bin6 ch6.c

clean:
	rm bin*