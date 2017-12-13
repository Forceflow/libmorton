CC=g++
CFLAGS=-O3 -m64 -std=c++14 -I ../libmorton/include/

all: test bmi2

test:
	$(CC) $(CFLAGS) libmorton_test.cpp -o libmorton_test

bmi2:
	$(CC) $(CFLAGS) -march=haswell libmorton_test.cpp -o libmorton_test_bmi2

clean:
	rm -f libmorton_test libmorton_test_bmi2
