#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include "time_util.h"

#define CSV_NAME "gen/cache_hierarchy.csv"
#define CACHE_LINE_SIZE 64       // Cache line size in bytes (64 bytes is typical)
#define ROWS 1024 * 1024 / 4    // Number of rows in the matrix
#define MATRIX_SIZE ROWS * CACHE_LINE_SIZE // Size of the matrix in bytes

FILE* make_csv() {
    FILE* file = fopen(CSV_NAME, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "size (bytes),latency (nsec)\n");
    return file;
}

void measure_cache_hierarchy() {
    struct timespec start, end;
    FILE* file = make_csv();

    volatile char** matrix = (volatile char**)malloc(ROWS * sizeof(char*));
    for (int i = 0; i < ROWS; i++) {
        matrix[i] = (volatile char*)malloc(CACHE_LINE_SIZE);
    }


    // Increment the working set size gradually
    for (int row = 0; row < ROWS; row++) {

        // Fill up caches with the matrix
        for (int i = 0; i < row; i++) {
            volatile char mid = matrix[i][31];
            (void)mid; // Prevent optimization
        }

        // Now measure the access latency of the first element
        clock_gettime(CLOCK_MONOTONIC, &start);
        memset((void*)matrix[0], 'a', CACHE_LINE_SIZE);
        clock_gettime(CLOCK_MONOTONIC, &end);

        // Calculate and write the latency to the CSV file
        double latency = timespec_to_nsec(difftimespec(end, start));
        unsigned long size = (row + 1) * CACHE_LINE_SIZE;
        fprintf(file, "%zu,%f\n", size, latency);
    }

    for (int i = 0; i < ROWS; i++) {
        free((void*)matrix[i]);
    }
    free(matrix);

    fclose(file);
}
