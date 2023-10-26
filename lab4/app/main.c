#include <stdlib.h>

#include "../include/tasks.h"

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (strncmp(argv[1], "1", 1) == 0)
            return task1();
        if (strncmp(argv[1], "2", 1) == 0)
            return task2();
        if (strncmp(argv[1], "3", 1) == 0)
            return task3();
        if (strncmp(argv[1], "4", 1) == 0)
            return task4();
        if (strncmp(argv[1], "5", 1) == 0)
            return task5();
    } else {
        printf("No arguments (1, 2, 3, 4 possible).\n");
        return 0;
    }
}
