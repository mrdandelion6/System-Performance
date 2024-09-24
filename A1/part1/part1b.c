#define _GNU_SOURCE

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "part1b.h"
#include "time_util.h"

// The following cache lines (in bytes) were determined by: 
// cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size  # L1d cache line size
// cat /sys/devices/system/cpu/cpu0/cache/index2/coherency_line_size  # L2 cache line size
// cat /sys/devices/system/cpu/cpu0/cache/index3/coherency_line_size  # L3 cache line size
#define L1d_cache_line 64
#define L2_cache_line 64
#define L3_cache_line 64
// Note that we were allowed to assume these numbers from the assignment's instructions as well.

// The following CSV file is used to persist the data collected from the Cache hierarchy experiment.
#define CSV_NAME "cache_hierachy.csv"

void make_csv() {
    // This function is invoked to create a CSV file. If the file already exists, all of its data will
    // be wiped and a new one blank one will be created. The CSV file has the columns "size", and
    // "latency".

    FILE* file = fopen(CSV_NAME, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
}   
    fprintf(file, "size", "latency");
    fclose(file);
}

void determine_cache_hierachy()  {
    // This function uses various input sizes to determine the cache hierarchy of the system. The
    // function writes the results to a CSV file. The function will write to an array in 
    // progressively larger chunks, and measure the time it takes to access each element. 

    // First we warm up the cache by accessing sections of the 1G array in order. We gradually
    // increase the size of the array we are accessing to determine the cache hierarchy.

}