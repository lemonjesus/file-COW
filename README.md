# File COW 🐄

A header-only C library for reading and writing files without making changes to the original file by using copy-on-write. I wrote this for my iPod QEMU emulator so I could read and write to the iPod's disk image without making changes to the original file or having to load the entire file into memory.

## Usage

```c
#include <file-cow.h>

// open a file
cow_file* cow = cow_open("file.txt");

// read the file - all reads and writes are random access
char* buf = malloc(100);
cow_read(cow, buf, 0, 100);

// write on top of the file (this does not change the original file)
cow_write(cow, "hello", 0, 5);

// read the file again, this time the first 5 bytes will be "hello" despite the original file not being changed
cow_read(cow, buf, 0, 100);

// if you want to commit the changes to the original file, unprotect the file and call cow_commit. optionally, call cow_protect to protect the file again to prevent accidental permanent changes. if you don't call cow_commit, the changes will be lost when the file is closed. by default, the file is protected.
cow_unprotect(cow);
cow_commit(cow);
cow_protect(cow);

// close the file, losing any uncommitted changes
cow_close(cow);
```

## Testing
Simply compile and run `test.c` to run tests. The tests are not exhaustive, but they do cover the basic functionality.

## Performance
Files aren't meant to be read from and written to randomly, so this library is not meant to be used for performance-critical applications. For my emulator, the disk image would need to be accessed randomly. For most copy-on-write things, I assume this will be the case anyway. That said, I haven't performance tested this, and I expect it to be slightly slower than regular file i/o.

In terms of memory, it consumes the amount of data submitted with `cow_write` plus a few bytes for structures. The original file is not loaded into memory. Once the file is closed or the changes are committed, the memory used to hold the changes is immediately freed. There is also no clever algorithm that detects overlapping changes, so if you write to the same location twice, it will consume twice the memory.

## Possible Future Work
- A clever algorithm that detects overlapping changes and compacts them
- A way to make a cow from a cow so you can layer them ad infinitum
- A way to undo changes, maybe out of order? (this would require switching to a linked list instead of an array of changes)

## License
~~MOO~~ MIT