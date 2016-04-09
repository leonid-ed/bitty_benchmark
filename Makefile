.PHONY : all clean

all: factorial_bench

#CXX=clang
CXX=gcc
CXXFLAGS=`cat flags`

BENCHMARK=bitty_benchmark.h

factorial_bench: flags factorial_bench.c $(BENCHMARK)
	$(CXX) $(CXXFLAGS) -o factorial_bench factorial_bench.c

perf: factorial_bench
	perf record -g ./factorial_bench
	perf report -g 'graph,0.5,caller'

clean:
	rm -f factorial_bench
	rm -f perf.*