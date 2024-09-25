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
#include "part1a.h"
#include "part1b.h"

int main(int argc, char *argv[])
{
	// Pin the thread to a single CPU to minimize the effects of scheduling
	// Don't use CPU #0 if possible, as it tends to be busier with servicing interrupts.
	srandom(time(NULL));
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET((random() ?: 1) % get_nprocs(), &set);
	if (sched_setaffinity(getpid(), sizeof(set), &set) != 0) {
		perror("sched_setaffinity");
		return 1;
	}

	// Part 1, A)
	// float bandwidth = measure_memory_write_bandwidth();
	// printf("bandwidth is %0.2f\n", bandwidth);

	// Part1, B)
	printf("Starting cache hierarchy experiment\n");
	measure_cache_hierarchy();

	return 0;
}
