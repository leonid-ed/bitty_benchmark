# Bitty Benchmark

Light benhmark kit based on C macros that you can use as one header file.

Benchmark results look as below:
```sh
$ ./factorial_bench
       61.29        61.29                       one   # Factorial measurement
     1000.09         0.02    1.0009 MB/s        one   # test IO speed
```

Result format is as following:
```sh
  elapsed  |  cpu-lock  | speed of        | benchmark | benchmark
  time,    |  time,     | data exchange   | id        | desription
  ms       |  ms        | (optional), MB/s|           | (optional)
-------------------------------------------------------------------------
   61.29   |     61.29  |                 |  one      | # Factorial measurement
 1000.09   |     0.02   | 1.0009 MB/s     |  one      | # test IO speed       
 ```
 
License
-------
MIT
