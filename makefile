CC=g++
CFLAGS=-O3 -m64 -std=c++11 -I libmorton/include/

all: libmorton_test

libmorton_test:
	$(CC) $(CFLAGS) test/libmorton_test.cpp
