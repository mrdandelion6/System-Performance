// ------------
// This code is provided solely for the personal and private use of
// students taking the CSC367H5 course at the University of Toronto.
// Copying for purposes other than this use is expressly prohibited.
// All forms of distribution of this code, whether as given or with
// any changes, are expressly prohibited.
//
// Authors: Bogdan Simion, Alexey Khrabrov
//
// All of the files in this directory and all subdirectories are:
// Copyright (c) 2022 Bogdan Simion
// -------------
#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <x86intrin.h>
#include <stdint.h>
#include "time_util.h"

#define CACHE_LINE_SIZE 64
// confirmed this value for lab machines
// in the case of a different machine, you can use the following 
// command to find the l3 cache line size: 
// getconf LEVEL3_DCACHE_LINESIZE

#define ROW_SIZE 2 * CACHE_LINE_SIZE
// This will be the number of bytes we read in at a time.
// This is set to be twice the cache line size because it
// will ensure that the bytes are retrieved directly from RAM
// while we iterate through a matrix.

typedef struct {
	volatile char data[ROW_SIZE];
} row_size_t;

row_size_t* allocate_matrix(int rows)
{
	// Allocate a matrix of size rows * ROW_LENGTH.
	// We purposely make the matrix contiguous.
	row_size_t* matrix = (row_size_t*)malloc(rows * sizeof(row_size_t));
	return matrix;
}

float measure_write_bandwidth()
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
	for (int i = 0; i < rows; i++) {
		memset(&matrix[i], INT32_MAX, sizeof(row_size_t));
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	free(matrix);

	float time_s = timespec_to_sec(difftimespec(end, start));
	printf("time was %f\n", time_s);

	size_t total_bytes = (size_t)rows * sizeof(row_size_t);
	float total_GB = total_bytes / (1024.0 * 1024.0 * 1024.0);

	printf("gigs were %f\n", total_GB);

	return (float)total_GB / time_s;
}

int main(int argc, char *argv[])
{
	// Pin the thread to a single CPU to minimize the effects of scheduling
	// Don't use CPU #0 if possible, as it tends to be busier with servicing interrupts
	srandom(time(NULL));
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET((random() ?: 1) % get_nprocs(), &set);
	if (sched_setaffinity(getpid(), sizeof(set), &set) != 0) {
		perror("sched_setaffinity");
		return 1;
	}

	//TODO
	float bandwidth = measure_write_bandwidth();
	printf("bandwidth is %0.2f\n", bandwidth);


	return 0;
}
