#include "../include/tasks.h"

int task1() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Child process is running.\n");
        exit(0);
    } else if (pid > 0) {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            printf("Child process exited with status: %d\n", exit_status);
        } else {
            printf("Child process exited abnormally.\n");
        }
    } else {
        perror("Fork failed");
        return 1;
    }

    return 0;
}

int task2() {
    struct dirent *entry;
    DIR *dp = opendir("/proc");

    if (dp == NULL) {
        perror("Error opening /proc");
        return 1;
    }

    while ((entry = readdir(dp))) {
        if (isdigit(entry->d_name[0])) {
            printf("PID: %s\n", entry->d_name);
        }
    }

    closedir(dp);
    return 0;
}

void *thread_function(void *arg) {
    for (int i = 0; i < 5; i++) {
        printf("meow\n");
        sleep(1);
    }
    return NULL;
}

int task3() {
    pthread_t threads[3];

    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, NULL) != 0) {
            perror("Thread creation failed");
            return 1;
        }
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

int task4() {
    struct dirent *entry;
    DIR *dp = opendir("/proc");

    if (dp == NULL) {
        perror("Error opening /proc");
        return 1;
    }

    while ((entry = readdir(dp))) {
        if (isdigit(entry->d_name[0])) {
            char path[256];
            snprintf(path, sizeof(path), "/proc/%s/maps", entry->d_name);

            FILE *file = fopen(path, "r");
            if (file) {
                char line[256];
                printf("Process PID: %s\n", entry->d_name);
                printf("Loaded Modules:\n");
                while (fgets(line, sizeof(line), file)) {
                    printf("%s", line);
                }
                fclose(file);
            }
        }
    }

    closedir(dp);
    return 0;
}

int task5() {
    const char *videoUrl = "https://www.youtube.com/watch?v=jNQXAC9IVRw&ab_channel=jawed";
    const int iter_nums = 5;

    for (int i = 0; i < iter_nums; i++) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Fork failed");
            exit(1);
        } else if (child_pid == 0) {
            char command[256];
            snprintf(command, sizeof(command), "google-chrome %s", videoUrl);
            printf("Playing video %d...\n", i + 1);
            int result = system(command);
            if (result != 0) {
                fprintf(stderr, "Error playing video %d\n", i + 1);
            }
            exit(0);
        } else {
            int status;
            waitpid(child_pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                printf("Video %d finished playing.\n", i + 1);
            } else {
                fprintf(stderr, "Error playing video %d\n", i + 1);
            }
        }
    }

    return 0;
}
