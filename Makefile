CC=gcc
CFLAGS=-I.

primes: primes.o
	$(CC) -o primes primes.o -I.
