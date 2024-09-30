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

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "data.h"
#include "time_util.h"

// We have 8 cores on the machine, so we can have up to 8 threads
// without oversubscribing the machine.
#define NUM_THREADS 8
#define CACHE_LINE_SIZE 64

pthread_t threads[NUM_THREADS];
int threads_made;

// This struct is passed into both thread functions sum_array and 
// compute_averages.
struct thread_args {
    course_record* courses;
    int start;
    int end;
    char padding[CACHE_LINE_SIZE - sizeof(course_record*) - 2 * sizeof(int)];
} __attribute__((aligned(CACHE_LINE_SIZE)));
// We're now aligning the thread args to fit nicely within two cache lines

// We store the arguments we will need to pass to each thread.
struct thread_args* args_array[NUM_THREADS];

void* compute_averages(void* args)
{
    struct thread_args* thread_args = (struct thread_args*)args;
    int start = thread_args->start;
    int end = thread_args->end;
    course_record* courses = thread_args->courses;

	// Keep track of the sum locally this time instead of repeatedly writing

    for (int i = start; i < end; i++) {
        assert(&courses[i] != NULL);
        assert(courses[i].grades != NULL);

        double course_sum = 0.0;
        int grades_count = courses[i].grades_count;
        grade_record* grades = courses[i].grades;

        for (int j = 0; j < grades_count; j++) {
            course_sum += grades[j].grade;
        }
        courses[i].average = course_sum / grades_count;
    }

    pthread_exit(NULL);
}

void start_parallel(course_record *courses, int courses_count)
{
    assert(courses != NULL);

    int courses_per_thread = (courses_count + NUM_THREADS - 1) / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        int start = i * courses_per_thread;
        int end = (i == NUM_THREADS - 1) ? courses_count : (start + courses_per_thread);

        if (start >= courses_count) break;

        struct thread_args* thread_args = (struct thread_args*)aligned_alloc(CACHE_LINE_SIZE, sizeof(struct thread_args));
        thread_args->courses = courses;
        thread_args->start = start;
        thread_args->end = end;
        args_array[i] = thread_args;
        threads_made++;
        pthread_create(&threads[i], NULL, compute_averages, (void*)thread_args);
    }

    for (int i = 0; i < threads_made; i++) {
        pthread_join(threads[i], NULL);
        free(args_array[i]);
    }
}

int main(int argc, char *argv[])
{
    course_record *courses;
    int courses_count;
    if (load_data((argc > 1) ? argv[1] : "part2data", &courses, &courses_count) < 0) return 1;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    start_parallel(courses, courses_count);
    clock_gettime(CLOCK_MONOTONIC, &end);

    for (int i = 0; i < courses_count; i++) {
        printf("%s: %f\n", courses[i].name, courses[i].average);
    }

	printf("%f\n", timespec_to_msec(difftimespec(end, start)));

    free_data(courses, courses_count);
    return 0;
}