# Cache Simulator

A C++ simulation of a 2-way set-associative CPU cache sitting in front of main memory. It models how a real cache splits addresses, detects hits and misses, evicts the least recently used line, and writes modified data back to memory.

## Cache Configuration

| Parameter | Value |
|---|---|
| Main memory | 32 KB (8,192 ints) |
| Cache size | 2 KB |
| Line size | 16 bytes (4 ints) |
| Associativity | 2-way set associative |
| Sets | 64 |
| Replacement policy | Least recently used (LRU) |
| Write policy | Write-back, write-allocate |

Memory and cache both start filled with `-1`, and every cache line starts invalid.

## How Addresses Are Split

Each 15-bit address is broken into three fields, with the bit widths calculated at startup from the cache parameters rather than hard-coded:

```
| tag (5 bits) | set index (6 bits) | offset (4 bits) |
```

The index picks one of the 64 sets, the tag is compared against both lines in that set, and the offset picks the byte within the 16-byte line.

## What It Does

On every access the simulator:

1. Splits the address into tag, index, and offset
2. Checks both lines in the set for a valid line with a matching tag (hit) or not (miss)
3. On a miss, picks a victim: an invalid line if one exists, otherwise the least recently used line
4. Writes the victim back to main memory if it is dirty
5. Loads the requested 16-byte block from memory into the cache
6. Updates the LRU timestamp, and for a write, stores the data and marks the line dirty

## Commands

| Command | Action |
|---|---|
| `A` | Access memory: enter an address, then `R` (read) or `W` (write, followed by an integer to store) |
| `B` | Display the memory value and both cache lines for an address |
| `B` with address `-1` | Quit |

Addresses for `A` are rounded down to a multiple of 4 so each access targets one int. Input is validated, including out-of-range addresses and non-numeric entries.

The `B` output is, in order: the address, the value in main memory, the value in each of the two cache lines (hex, or `-1` if that line is empty), then a valid bit and dirty bit for each line.

## Sample Session

Addresses `0`, `1024`, and `2048` all map to set 0, so the third write forces an eviction. This shows the write-back in action: after the eviction, the `111` written to address 0 (`0x6f`) shows up in main memory.

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

Before the third write, main memory still holds the initial `ffffffff` because the cache has not written anything back yet. The write to 2048 evicts address 0 (the least recently used line in the set), which is dirty, so `0x6f` is flushed to memory and its slot now holds `0x14d` (333).

## Concepts Demonstrated

- Set-associative cache organization and address decomposition (tag, index, offset)
- LRU replacement using access timestamps
- Write-back and write-allocate policies with dirty bits
- Bitwise operations for masking and shifting addresses
- Structs, 2D arrays, and global state in C++
- Input validation

## Build and Run

There is no platform-specific code, so it builds anywhere with a C++ compiler. It was originally developed in Visual Studio on Windows.

**macOS / Linux (terminal or VS Code's integrated terminal):**

```
g++ -std=c++11 cache_simulator.cpp -o cache_simulator
./cache_simulator
```

**Windows (Visual Studio):** create a new C++ Console App project, add `cache_simulator.cpp`, and run.

## Notes

- Reads do not print the data. Use command `B` to inspect memory and cache state after any access.
- The LRU selection logic compares the two ways in a set, so it is written specifically for 2-way associativity. Changing `associativity` would require generalizing that function.
