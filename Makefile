CC=musl-gcc
CFLAGS=-static -s -O3 -std=c99 -pedantic -Wall

wedge: wedge.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm wedge

