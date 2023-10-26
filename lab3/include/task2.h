#ifndef TASK2_H
#define TASK2_H

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void sort_file_alphabetically(const char *filename);
int compare_strings(const void *a, const void *b);
void count_letters(const char *filename);
void remove_and_count_a(const char *filename);
void sort_numeric_descending(const char *filename);
int reverse_numeric_compare(const void *a, const void *b);
#endif