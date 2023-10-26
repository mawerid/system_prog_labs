#ifndef TASK1_H
#define TASK1_H

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

void print_help();
void create_file(const char *filename, const char *content);
void read_file(const char *filename);
void delete_file(const char *filename);
void rename_file(const char *filename, const char *newname);
void copy_file(const char *src, const char *dest);
void display_file_size(const char *filename);
void display_file_attributes(const char *filename);
void set_read_only(const char *filename);
void set_hidden(const char *filename);
void list_directory_contents(const char *dirname);
#endif