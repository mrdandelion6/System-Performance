#define L3_CACHE_LINE_SIZE 64
// confirmed this value for lab machines
// in the case of a different machine, you can use the following 
// command to find the l3 cache line size: 
// getconf LEVEL3_DCACHE_LINESIZE

typedef struct {
	char data[2 * L3_CACHE_LINE_SIZE];
} row_size_t;

float measure_memory_write_bandwidth();