# ------------
# This code is provided solely for the personal and private use of
# students taking the CSC367H5 course at the University of Toronto.
# Copying for purposes other than this use is expressly prohibited.
# All forms of distribution of this code, whether as given or with
# any changes, are expressly prohibited.
#
# Authors: Bogdan Simion, Alexey Khrabrov
#
# All of the files in this directory and all subdirectories are:
# Copyright (c) 2022 Bogdan Simion
# -------------

CC = gcc
CFLAGS += -std=gnu11 -Wall -Werror -g3 -O3 -DNDEBUG -pg -fprefetch-loop-arrays

# objects
OBJS = src/part1.o src/part1a.o src/part1b.o

# target executable
TARGET = part1

all: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#TODO: run your script that runs the experiments, collects all necessary data, and produces the graphs
run: all
	./my_script.sh

part1: src/part1.o
	$(CC) $(CFLAGS) $^ -o $@

# Run perf for L1D cache accesses
perf_l1d_access: all
	perf stat -e L1-dcache-loads ./$(TARGET)

# Run perf for L1D cache misses
perf_l1d_miss: all
	perf stat -e L1-dcache-load-misses ./$(TARGET)

clean:
	rm -f src/*.o part1 src/perf.data src/perf.data.old gmon.out
