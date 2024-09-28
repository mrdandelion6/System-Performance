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
pthread_t threads[NUM_THREADS];
int threads_made;

// This struct is passed into both thread functions sum_array and 
// compute_averages.
struct thread_args {
	course_record* courses;
	int start;
	int end;
	int result;
	int id;
};

// We store the arguments we will need to pass to each thread.
struct thread_args* args_array[NUM_THREADS];


void combine_sums(course_record* courses, int courses_count) {
	int threads_per_course = NUM_THREADS / courses_count;

	// Join the results of all the threads together
	for (int i = 0; i < courses_count; i++) {
		int start = i * threads_per_course;
		int end = (i == courses_count - 1) ? NUM_THREADS: threads_per_course;
		int total = 0;
		for (int i = start; i < end; i++) {
			total += args_array[i]->result;
		}
	}
}

// Simply sum an array. This function will be executed if multiple
// threads work on a single course.
void* sum_array(void* args) 
{
	int start = ((struct thread_args*)args)->start;
	int end = ((struct thread_args*)args)->end;
	int result = ((struct thread_args*)args)->result;
	course_record* courses = ((struct thread_args*)args)->courses;
	
	result = 0;
	for (int i = start; i < end; i++) {
		result += courses->grades[i].grade;
	}

	printf("result is %d\n", result);

	pthread_exit(NULL);
}

// Compute the historic average grade for <num_courses> courses. 
// Updates the average value in the record.
void* compute_averages(void* args)
{
	int start = ((struct thread_args*)args)->start;
	int end = ((struct thread_args*)args)->end;
	int id = ((struct thread_args*)args)->id;
	course_record* courses = ((struct thread_args*)args)->courses;

	for (int i = start; i < end; i++) {
		assert(&courses[i] != NULL);
		assert(courses[i].grades != NULL);

		courses[i].average = 0.0;
		for (int j = 0; j < courses[i].grades_count; j++) {
			courses[i].average += courses[i].grades[j].grade;
		}
		courses[i].average /= courses[i].grades_count;
	}
	pthread_exit(NULL);
}

// Compute the historic average grades for all the courses.
void start_parallel(course_record *courses, int courses_count)
{
	assert(courses != NULL);

	// We either have multiple threads working on each task, or 
	// multiple tasks for each thread to complete.
	int more_eq_courses = (courses_count >= NUM_THREADS);
	int dividend = more_eq_courses ?  courses_count : NUM_THREADS; 
	int divisor = more_eq_courses ?  NUM_THREADS : courses_count;
	// Note that divisor <= THREADS_MAX
	void* (*f)(void*) = more_eq_courses ? compute_averages : sum_array;

	int dividend_chunk = dividend / divisor;

	for (int i = 0; i < divisor; i++) {
		int start = i * dividend_chunk;
		// Handle remainder
		int end = (i == divisor - 1) ? dividend : start + dividend_chunk;

		// Create struct for passing args to our function.
		struct thread_args* thread_args = (struct thread_args*)malloc(sizeof(struct thread_args));
		thread_args->courses = courses;
		thread_args->start = start;
		thread_args->end = end;
		thread_args->id = i;
		args_array[i] = thread_args;
		threads_made++;
		pthread_create(&threads[i], NULL, f, (void*)thread_args);
	}

	for (int i = 0; i < threads_made; i++) {
		pthread_join(threads[i], NULL);
	} 

	if (!more_eq_courses) {
		// Have to join the partial sums together because we had multiple threads to each course
		combine_sums(courses, courses_count);
	}

	// Free all the thread arguments.
	for (int i = 0; i < divisor; i++) {
		free(args_array[i]);
	}
}

int main(int argc, char *argv[])
{
	course_record *courses;
	int courses_count;
	// Load data from file; "part2data" is the default file path if not specified
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
