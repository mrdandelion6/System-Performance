#define _GNU_SOURCE

#include <x86intrin.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "part1a.h"
#include "time_util.h"

row_size_t* allocate_matrix(int rows)
{
	// Allocate a matrix of size rows * ROW_LENGTH.
	// We purposely make the matrix contiguous.
	// We use posix_memalign for efficient contiguous memory alignment.
    row_size_t* matrix;
    if (posix_memalign((void**)&matrix, L3_CACHE_LINE_SIZE, rows * sizeof(row_size_t)) != 0) {
        perror("posix_memalign");
        exit(1);
    }
	return matrix;
}

void write_to_memory(row_size_t* matrix, int rows, int simd) {
	// This function writes values into matrix to test memory 
	// bandwidth for measure_write_bandwidth(). Includes an SIMD 
	// option as it may write to memory more efficiently.

	if (simd) {
		__m128i value = _mm_set1_epi32(INT32_MAX);
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < sizeof(row_size_t) / sizeof(__m128i); j++) {
				_mm_store_si128((__m128i*)&matrix[i].data[j * sizeof(__m128i)], value);
			}
    	}
	} else {
		for (int i = 0; i < rows; i++) {
			memset(&matrix[i], INT32_MAX, sizeof(row_size_t));
		}	
	}
	
}

float measure_memory_write_bandwidth()
{
	// This function tests the limits of the memory bus by writing to memory (RAM)
	// as fast as possible. We allocate a matrix where each row has a size greater 
	// than the computer's cache line. We then traverse this matrix going down each
	// column to purposely miss our L1, L2, and L3 caches. At each iteration, we
	// write to an entire row at once using memset. So this matrix can be thought 
	// more of as an array of chunks instead.
	// Returns bandwidth in GB/s.

	int rows = 100000000;
	row_size_t* matrix = allocate_matrix(rows);

	struct timespec  start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
	write_to_memory(matrix, rows, 0);
	clock_gettime(CLOCK_MONOTONIC, &end);

	free(matrix);

	float time_s = timespec_to_sec(difftimespec(end, start));
	printf("time was %f\n", time_s);

	size_t total_bytes = (size_t)rows * sizeof(row_size_t);
	float total_GB = total_bytes / (1024.0 * 1024.0 * 1024.0);

	printf("gigs were %f\n", total_GB);

	return (float)total_GB / time_s;
}