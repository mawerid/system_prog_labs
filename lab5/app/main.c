#include <stdlib.h>

#include "../include/tasks.h"

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (strncmp(argv[1], "1", 1) == 0)
            return task1(argc, argv);
        if (strncmp(argv[1], "2", 1) == 0)
            return task2(argc, argv);
        if (strncmp(argv[1], "3", 1) == 0)
            return task3();
    } else {
        printf("No arguments (1, 2, 3 possible).\n");
        return 0;
    }
}
