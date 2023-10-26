#ifndef TASKS_H
#define TASKS_H

#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

int task1(int argc, char *argv[]);
int task2(int argc, char *argv[]);
int task3();
#endif