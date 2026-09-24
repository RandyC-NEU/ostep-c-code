CC = gcc
CFLAGS = -Wall -Wpedantic -O0

.PHONY: all clean

all: build_5 build_6 build_7

build_5: ch5.c
	$(CC) $(CFLAGS) -o bin5 ch5.c

build_6: ch6.c
	$(CC) $(CFLAGS) -o bin6 ch6.c

build_7: ch7.c
	$(CC) $(CFLAGS) -o bin7 ch7.c

clean:
	rm bin*