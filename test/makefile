CXX=g++
CFLAGS=-O3 -m64 -I ../include

all: test bmi2 avx512 zen2

test:
	$(CXX) $(CFLAGS) libmorton_test.cpp -o libmorton_test

bmi2:
	$(CXX) $(CFLAGS) -march=haswell libmorton_test.cpp -o libmorton_test_bmi2

avx512:
	$(CXX) $(CFLAGS) -march=icelake-client libmorton_test.cpp -o libmorton_test_avx512

clean:
	rm -f libmorton_test libmorton_test_bmi2 libmorton_test_avx512 libmorton_test_zen2

zen2:
	$(CXX) $(CFLAGS) -march=znver2 libmorton_test.cpp -o libmorton_test_zen2
