# Custom Heap Memory Manager

## Overview
This project implements a custom heap memory manager in C, offering basic functionalities like memory allocation (`malloc`), deallocation (`free`), and reallocation (`realloc`). Additionally, it includes utilities for writing to memory and printing memory blocks. The manager operates on a fixed-size heap and employs simple strategies such as best-fit for allocation and block coalescing for deallocation.

## Features
- **Heap Initialization:** Initializes the heap with a specified size, marking it entirely free.
- **Memory Allocation (`malloc`):** Allocates a memory block of a specified size using a best-fit strategy. Splits blocks if necessary.
- **Memory Deallocation (`free`):** Frees a previously allocated memory block, merging adjacent free blocks to mitigate fragmentation.
- **Memory Reallocation (`realloc`):** Adjusts the size of an allocated memory block, moving data to a new location if in-place resizing is not feasible.
- **Memory Writing (`writemem`):** Writes data to a specific heap location.
- **Memory Block Listing (`blocklist`):** Displays all memory blocks, their sizes, and allocation status.
- **Memory Printing (`printmem`):** Prints memory content starting from a specified address.

## Compilation and Execution
Compile with GCC:
```sh
gcc -o memory_manager memory_manager.c
```
Run the executable:

```
./memory_manager
```
## Usage
The program runs in interactive mode, accepting commands to manage memory:

`malloc <size>`: Allocates memory of the given size.
`free <address>`: Frees the memory block at the specified address.
`realloc <address> <newsize>`: Resizes the memory block to the new size.
blocklist: Lists all memory blocks and their statuses.
`writemem <address> <data>`: Writes data to the specified memory address.
`printmem <address> <length>`: Prints the contents of memory from the given address.
quit: Exits the program.
### Notes
The heap size is fixed at compile time, defined as HEAP_SIZE.
Focuses on functionality over performance with simple memory management implementations.
Does not handle memory alignment; allocations are byte-aligned.
Terminates upon detecting corrupted heap headers to prevent undefined behavior.
Authors
* Mehmet Nadi
* Khoi Pham
