# Bitty Benchmark

Light benhmark kit based on C macros that you can use as one header file.

Benchmark results look as below:
```sh
$ ./factorial_bench
      698.26       688.96                       one   # Factorial with recursion
     1000.75         0.03    1.0002 MB/s        one   # Test IO speed
      181.65       170.42                       one   # Factorial with memoization
```

Result format is as following:
```sh
  elapsed  |  cpu-lock  | speed of        | benchmark | benchmark
  time,    |  time,     | data exchange   | id        | desription
  ms       |  ms        | (optional), MB/s|           | (optional)
-------------------------------------------------------------------------
  698.26   |    688.96  |                 |  one      | # Factorial with recursion
 1000.09   |      0.03  | 1.0002 MB/s     |  one      | # Test IO speed
  181.65   |    170.42  |                 |  one      | # Factorial with memoization
 ```

License
-------
MIT
