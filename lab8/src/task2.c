#include "../include/task2.h"

void sort_file_alphabetically(const char *filename) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error getting file size");
        close(fd);
        return;
    }

    char *data = (char *)mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return;
    }

    qsort(data, st.st_size, sizeof(char), compare_strings);

    munmap(data, st.st_size);
    close(fd);

    printf("File contents sorted alphabetically.\n");
}

int compare_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void count_letters(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error getting file size");
        close(fd);
        return;
    }

    char *data = (char *)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return;
    }

    int lowercase_count = 0;
    int uppercase_count = 0;

    for (size_t i = 0; i < st.st_size; i++) {
        if (islower(data[i])) {
            lowercase_count++;
        } else if (isupper(data[i])) {
            uppercase_count++;
        }
    }

    munmap(data, st.st_size);
    close(fd);

    printf("Lowercase letters: %d\n", lowercase_count);
    printf("Uppercase letters: %d\n", uppercase_count);
}

void remove_and_count_a(const char *filename) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error getting file size");
        close(fd);
        return;
    }

    char *data = (char *)mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return;
    }

    int a_count = 0;
    for (size_t i = 0; i < st.st_size; i++) {
        if (data[i] == 'a' || data[i] == 'A') {
            a_count++;
            data[i] = ' ';
        }
    }

    char a_count_str[32];
    snprintf(a_count_str, sizeof(a_count_str), "Removed %d 'a's", a_count);
    strcat(data, a_count_str);

    munmap(data, st.st_size + strlen(a_count_str));
    close(fd);

    printf("Removed %d 'a's from the file.\n", a_count);
}

int reverse_numeric_compare(const void *a, const void *b) {
    return atoi(*(const char **)b) - atoi(*(const char **)a);
}

void sort_numeric_descending(const char *filename) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error getting file size");
        close(fd);
        return;
    }

    char *data = (char *)mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return;
    }

    char *lines[1024];
    int line_count = 0;

    char *token = strtok(data, "\n");
    while (token != NULL) {
        lines[line_count] = token;
        line_count++;
        token = strtok(NULL, "\n");
    }

    qsort(lines, line_count, sizeof(char *), reverse_numeric_compare);

    char new_content[st.st_size];
    char *current_pos = new_content;
    for (int i = 0; i < line_count; i++) {
        strcpy(current_pos, lines[i]);
        current_pos += strlen(lines[i]);
        *current_pos = '\n';
        current_pos++;
    }

    lseek(fd, 0, SEEK_SET);
    write(fd, new_content, current_pos - new_content);

    munmap(data, st.st_size);
    close(fd);

    printf("File contents sorted in descending order.\n");
}