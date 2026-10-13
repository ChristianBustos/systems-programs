# Cache Simulator

A C++ program that simulates a small 2-way set-associative cache sitting in front of main memory. You can read and write addresses, then look at what ended up in the cache and in memory.

## Cache Setup

- 32 KB main memory
- 2 KB cache with 16-byte lines (4 ints each)
- 2-way set associative, 64 sets
- LRU replacement
- Write-back and write-allocate

Memory and cache start filled with -1, and every cache line starts out invalid.

## How It Works

Each 15-bit address is split into a 5-bit tag, a 6-bit set index, and a 4-bit offset. The bit widths are calculated when the program starts instead of being hard-coded.

On every access:

1. The index picks a set, and both lines in that set are checked for a valid line with a matching tag.
2. On a miss, an empty line is used if there is one. If not, the least recently used line is evicted, and if it is dirty it gets written back to memory first.
3. The 16-byte block is loaded from memory into the cache.
4. A read leaves the line in the cache. A write changes the value and marks the line dirty.

## Commands

- `A` - access memory. Enter an address, then `R` (read) or `W` (write, which also asks for an integer to store). Addresses are rounded down to a multiple of 4.
- `B` - display an address, showing the value in memory and in both cache lines.
- `B` with address `-1` - quit.

The display line shows the address, the value in memory, the value in each of the two cache lines (in hex, or `-1` if the line is empty), then the valid and dirty bits for each line.

The program checks for bad command letters, bad R/W entries, and addresses outside 0 to 32767.

## Sample Session

Addresses 0, 1024, and 2048 all land in set 0, so the third write forces an eviction.

```
Enter a command A or B: A
Enter the address: 0
Enter R to read memory or W to write memory: W
Enter integer data to be written: 111

Enter a command A or B: A
Enter the address: 1024
Enter R to read memory or W to write memory: W
Enter integer data to be written: 222

Enter a command A or B: B
Enter the address: 0
Address: 0 memory:ffffffff 0000006f 000000de 1 1 1 1

Enter a command A or B: A
Enter the address: 2048
Enter R to read memory or W to write memory: W
Enter integer data to be written: 333

Enter a command A or B: B
Enter the address: 0
Address: 0 memory:0000006f 0000014d 000000de 1 1 1 1
```

At first, memory still shows `ffffffff` because nothing has been written back yet. Writing to 2048 evicts the oldest line in the set (address 0), which is dirty, so its value `0x6f` (111) is written to memory.

## Concepts Demonstrated

- Set-associative cache (tag, index, offset)
- LRU replacement
- Write-back and write-allocate with dirty bits
- Bit masking and shifting
- Structs and 2D arrays

## Build and Run

I wrote this in Visual Studio on Windows, but it has no platform-specific code, so it also builds with g++ on Mac or Linux:

```
g++ -std=c++11 cache_simulator.cpp -o cache_simulator
./cache_simulator
```

## Notes

- Reads don't print the value. Use `B` to look at the cache and memory afterward.
- The LRU code compares the two ways in a set, so it only works for 2-way associativity.
- Entering letters where a number is expected isn't handled.
