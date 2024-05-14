# Serf
```
   _____           ____
  / ___/___  _____/ __/
  \__ \/ _ \/ ___/ /_  
 ___/ /  __/ /  / __/  
/____/\___/_/  /_/
```
Streaming error-bound compressor.

## How to generate project build-system
`cmake -DCMAKE_BUILD_TYPE=release -B build`

## How to build Serf and test
`cmake --build build && ./build/bin/serf_test`

## Baseline

- ALP
- Buff
- Chimp128
- Deflate
- Elf
- FPC
- Gorilla
- LZ4
- LZ77
- LZW
- Machete
- MOST
- Serf
- SZ1.0

## Python interface
Install pybind11 using pip.