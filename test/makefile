CFLAGS=-O3 -m64 -std=c++11 -I ../libmorton/include/

all: test bmi2 avx512

test:
	$(CC) $(CFLAGS) libmorton_test.cpp -o libmorton_test

bmi2:
	$(CC) $(CFLAGS) -march=haswell libmorton_test.cpp -o libmorton_test_bmi2

avx512:
	$(CC) $(CFLAGS) -march=icelake-client libmorton_test.cpp -o libmorton_test_avx512

clean:
	rm -f libmorton_test libmorton_test_bmi2 libmorton_test_avx512
