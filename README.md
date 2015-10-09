# CS4223_MESI_Dragon_Simulator

### Input:
Multiple trace files for different cores:
The trace files are ASCII data files (trace files, which have the extension “.prg”) consisting of lines where each one has two numbers, separated by only one white space: **Label** **Value**

- Label is a decimal number that identifies the memory access operation type demanded by the processor in a given time: fetch instruction (0), read memory data (2) or write a data in memory (3).

- Value is a 32-bit hexadecimal number that indicates the effective address of the memory word to be accessed by the processor
The figure above illustrates an example of trace file generated by one processor. The part of the file in the figure shows a memory trace with 6 instructions. In addition, there are 3 data reads and 1 data write. In total, those 6 instructions imply 10 memory accesses.


======================================================

### Assumptions:

1. Memory address is 32-bit
2. Each memory reference accesses 16-bit (2-bytes) of data. That is word size is 2- bytes.
3. We are only interested in data cache and instruction references can be ignored.
4. Each processor has its own L1 data cache.
5. L1 data cache uses **write-back**, write-allocate policy and **LRU replacement** policy. 
6. L1 data caches are kept coherent using **cache coherence protocol**.
7. Initially all caches are empty.
8. The bus uses **first come first serve** arbitration policy when multiple processor attempt bus transactions simultaneously. Ties are broken arbitrarily.
9. The L1 data caches are backed up by main memory --- there is no L2 data cache.
10. L1 cache hit is **1 cycle**. Fetching a block **from memory** to cache takes additional **100 cycles**. Sending **a word** from one cache to another (BusUpdate) or BusRdX takes only **1 cycle**. However, sending a **cache block with N words** (each word is 2 bytes) to another cache takes **N cycle**. Assume that **evicting a dirty cache block to memory** when it gets replaced is **100 cycles**.
11. You may need to make additional assumptions. Clearly state those assumptions in your report.￼￼￼￼￼
12. Also assume that the **caches are blocking**. That is, if there is a cache miss, the cache cannot process further requests from the processor. However, the snooping transactions from the bus still need to be processed.
13. In each cycle, each processor can make **at most one instruction reference and at most one memory reference**. As per our assumptions, you do not need to model L1 instruction cache. So the instruction references address can simply be ignored; but the cycle counter still has to be incremented.

=============================================================

### Basic Tasks:
######Implement MESI invalidation-based cache coherence protocol and Dragon 4-state update-based cache coherence protocols

Command line should be:
*`coherence <protocol> <input_file> <no_processors> <cache_size> <associativity> <block_size>`*

Default parameters as 4 processors, 16-bit word size, 16-byte block size, and 4KB direct-mapped cache per processor.

==============================================================

### Output:

1. Data cache miss rate for each processor
2. Amount of Data traffic in bytes on the bus 
3. Execution cycles per processor

==============================================================

### Advanced Tasks:

1. What is the impact of cache size? Vary cache size between 1KB and 32KB.
2. What is the impact of associativity? Vary associativity as 1, 2, 4
3. What is the impact of the number of processors? Vary number of processors as 1, 2, 4, and 8
4. What is the impact of block size? Vary block size between 8 byte and 128 byte
5. You may perform a quick literature survey to identify some optimizations to the basic MESI and Dragon protocol. Implement one optimization and evaluate the improvement if any.
